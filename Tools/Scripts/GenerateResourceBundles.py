#!/usr/bin/env python3
"""
Generate Java ResourceBundle properties files from WinMerge RC + PO sources.

Inputs:
  - Src/Merge.rc
  - Translations/WinMerge/*.po

Outputs:
  - winmerge-desktop/src/main/resources/i18n/WinMerge.properties
  - winmerge-desktop/src/main/resources/i18n/WinMerge_<locale>.properties
"""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
import re
from typing import Dict, List, Optional, Tuple


ROOT = Path(__file__).resolve().parents[2]
RC_PATH = ROOT / "Src" / "Merge.rc"
PO_DIR = ROOT / "Translations" / "WinMerge"
OUT_DIR = ROOT / "winmerge-desktop" / "src" / "main" / "resources" / "i18n"

NC_EXPR = re.compile(r'NC_\s*\(\s*"((?:[^"]|"")*)"\s*,\s*"((?:[^"]|"")*)"\s*\)')
QUOTED_EXPR = re.compile(r'"((?:[^"]|"")*)"')
KEY_LINE = re.compile(r"^([A-Za-z0-9_]+)\b(.*)$")

PO_FIELD = re.compile(r'^(msgctxt|msgid|msgstr)\s+"(.*)"\s*$')
PO_CONT = re.compile(r'^"(.*)"\s*$')

FALLBACK_LOCALES = {
    "Basque": "eu",
    "Croatian": "hr",
    "Czech": "cs",
    "Greek": "el",
    "Polish": "pl",
    "Russian": "ru",
    "Serbian": "sr",
    "Sinhala": "si",
}


@dataclass(frozen=True)
class RcEntry:
    key: str
    context: Optional[str]
    msgid: str


def decode_c_escapes(text: str) -> str:
    out: List[str] = []
    i = 0
    while i < len(text):
        ch = text[i]
        if ch != "\\":
            out.append(ch)
            i += 1
            continue

        if i + 1 >= len(text):
            out.append("\\")
            break

        nxt = text[i + 1]
        i += 2
        mapping = {
            "n": "\n",
            "r": "\r",
            "t": "\t",
            "b": "\b",
            "f": "\f",
            "a": "\a",
            "v": "\v",
            "\\": "\\",
            '"': '"',
            "'": "'",
            "?": "?",
        }
        if nxt in mapping:
            out.append(mapping[nxt])
            continue
        if nxt == "x":
            hex_digits: List[str] = []
            while i < len(text) and text[i] in "0123456789abcdefABCDEF":
                hex_digits.append(text[i])
                i += 1
            if hex_digits:
                out.append(chr(int("".join(hex_digits), 16)))
            else:
                out.append("x")
            continue
        if nxt in "01234567":
            oct_digits = [nxt]
            for _ in range(2):
                if i < len(text) and text[i] in "01234567":
                    oct_digits.append(text[i])
                    i += 1
                else:
                    break
            out.append(chr(int("".join(oct_digits), 8)))
            continue
        out.append(nxt)
    return "".join(out)


def parse_rc_value(expr: str) -> Tuple[Optional[str], str]:
    nc = NC_EXPR.search(expr)
    if nc:
        ctx = decode_c_escapes(nc.group(1).replace('""', '"'))
        msgid = decode_c_escapes(nc.group(2).replace('""', '"'))
        return ctx, msgid

    q = QUOTED_EXPR.search(expr)
    if not q:
        raise ValueError(f"Cannot parse RC value expression: {expr!r}")
    msgid = decode_c_escapes(q.group(1).replace('""', '"'))
    return None, msgid


def parse_rc(path: Path) -> List[RcEntry]:
    lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    entries: List[RcEntry] = []
    by_key: Dict[str, RcEntry] = {}

    in_stringtable: bool | str = False
    pending_key: Optional[str] = None

    for line in lines:
        s = line.strip()
        if s == "STRINGTABLE":
            in_stringtable = "expect_begin"
            pending_key = None
            continue

        if in_stringtable == "expect_begin":
            in_stringtable = s == "BEGIN"
            continue

        if in_stringtable is not True:
            continue

        if s == "END":
            in_stringtable = False
            pending_key = None
            continue
        if not s or s.startswith("//"):
            continue

        if pending_key is not None:
            key = pending_key
            expr = s
            pending_key = None
        else:
            m = KEY_LINE.match(s)
            if not m:
                continue
            key = m.group(1)
            expr = m.group(2).strip()
            if '"' not in expr and "NC_(" not in expr:
                pending_key = key
                continue

        context, msgid = parse_rc_value(expr)
        entry = RcEntry(key=key, context=context, msgid=msgid)
        prev = by_key.get(key)
        if prev is None:
            by_key[key] = entry
            entries.append(entry)
        elif prev != entry:
            raise ValueError(f"Conflicting RC entry for {key}: {prev} vs {entry}")

    if pending_key is not None:
        raise ValueError(f"Dangling RC key without value: {pending_key}")

    return entries


def parse_po_string(raw: str) -> str:
    return decode_c_escapes(raw)


def parse_header_language(header: str) -> Optional[str]:
    for line in header.split("\n"):
        if line.startswith("Language:"):
            code = line[len("Language:") :].strip()
            if code:
                return code
    return None


def parse_po(path: Path) -> Tuple[Dict[Tuple[Optional[str], str], str], Optional[str]]:
    entries: Dict[Tuple[Optional[str], str], str] = {}
    language: Optional[str] = None

    current_ctx = ""
    current_id: Optional[str] = None
    current_str: Optional[str] = None
    active_field: Optional[str] = None
    obsolete = False

    def flush() -> None:
        nonlocal current_ctx, current_id, current_str, active_field, obsolete, language
        if current_id is not None and not obsolete:
            key = (current_ctx or None, current_id)
            entries[key] = current_str or ""
            if current_id == "":
                language = parse_header_language(current_str or "")
        current_ctx = ""
        current_id = None
        current_str = None
        active_field = None
        obsolete = False

    for line in path.read_text(encoding="utf-8", errors="replace").splitlines() + [""]:
        t = line.strip()
        if t.startswith("#~"):
            obsolete = True
            continue
        if t == "":
            flush()
            continue
        if obsolete:
            continue
        if t.startswith("#"):
            continue

        f = PO_FIELD.match(t)
        if f:
            field, raw = f.group(1), f.group(2)
            val = parse_po_string(raw)
            if field == "msgctxt":
                current_ctx = val
                active_field = "ctx"
            elif field == "msgid":
                current_id = val
                active_field = "id"
            elif field == "msgstr":
                current_str = val
                active_field = "str"
            continue

        c = PO_CONT.match(t)
        if c:
            val = parse_po_string(c.group(1))
            if active_field == "ctx":
                current_ctx += val
            elif active_field == "id":
                current_id = (current_id or "") + val
            elif active_field == "str":
                current_str = (current_str or "") + val

    return entries, language


def normalize_locale(code: str) -> str:
    return code.replace("-", "_")


def escape_property_key(key: str) -> str:
    escaped = []
    for idx, ch in enumerate(key):
        if ch in ("\\", "=", ":", "#", "!"):
            escaped.append("\\" + ch)
        elif idx == 0 and ch == " ":
            escaped.append("\\ ")
        else:
            escaped.append(ch)
    return "".join(escaped)


def escape_property_value(value: str) -> str:
    escaped: List[str] = []
    for idx, ch in enumerate(value):
        if ch == "\\":
            escaped.append("\\\\")
        elif ch == "\t":
            escaped.append("\\t")
        elif ch == "\n":
            escaped.append("\\n")
        elif ch == "\r":
            escaped.append("\\r")
        elif ch == "\f":
            escaped.append("\\f")
        elif ch in ("=", ":", "#", "!"):
            escaped.append("\\" + ch)
        elif idx == 0 and ch == " ":
            escaped.append("\\ ")
        else:
            escaped.append(ch)
    return "".join(escaped)


def write_properties(path: Path, ordered_keys: List[str], values: Dict[str, str]) -> None:
    lines = [f"{escape_property_key(key)}={escape_property_value(values.get(key, ''))}" for key in ordered_keys]
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def build() -> None:
    rc_entries = parse_rc(RC_PATH)
    ordered_keys = [e.key for e in rc_entries]
    base_values = {e.key: e.msgid for e in rc_entries}
    rc_lookup = {e.key: e for e in rc_entries}

    # Ticket AMP-14 acceptance criteria references IDS_APP_TITLE explicitly.
    if "AFX_IDS_APP_TITLE" in base_values and "IDS_APP_TITLE" not in base_values:
        ordered_keys.append("IDS_APP_TITLE")
        base_values["IDS_APP_TITLE"] = base_values["AFX_IDS_APP_TITLE"]

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    write_properties(OUT_DIR / "WinMerge.properties", ordered_keys, base_values)

    po_files = sorted(PO_DIR.glob("*.po"))
    generated_locales: List[str] = []

    for po_file in po_files:
        po_map, language = parse_po(po_file)
        locale = normalize_locale(language) if language else FALLBACK_LOCALES.get(po_file.stem, po_file.stem.lower())

        values: Dict[str, str] = {}
        for key in ordered_keys:
            if key == "IDS_APP_TITLE":
                values[key] = base_values[key]
                continue

            entry = rc_lookup[key]
            translated = po_map.get((entry.context, entry.msgid))
            values[key] = translated if translated else entry.msgid

        write_properties(OUT_DIR / f"WinMerge_{locale}.properties", ordered_keys, values)
        generated_locales.append(locale)

    ids_keys = [k for k in ordered_keys if k.startswith("IDS_")]
    missing_ids = [k for k in ids_keys if k not in base_values]
    if missing_ids:
        raise ValueError(f"Missing IDS_* keys in base properties: {missing_ids}")

    print(f"Parsed RC STRINGTABLE entries: {len(rc_entries)}")
    print(f"IDS_* keys in RC: {len([k for k in [e.key for e in rc_entries] if k.startswith('IDS_')])}")
    print(f"Generated base bundle: {OUT_DIR / 'WinMerge.properties'}")
    print(f"Generated locale bundles: {len(generated_locales)}")
    print(f"Locales: {', '.join(generated_locales)}")


if __name__ == "__main__":
    build()
