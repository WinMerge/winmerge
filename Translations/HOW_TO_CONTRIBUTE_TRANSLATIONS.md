# How to Contribute Translations to WinMerge

Thank you for your interest in improving WinMerge translations! This guide will help you contribute translations effectively.

## 📊 Current Translation Status

Check [TranslationsStatus.md](TranslationsStatus.md) to see which languages need updates.

**Languages needing help (as of Oct 2025):**
- 🔴 **High Priority** (< 50% complete): Ukrainian (47%), Arabic (54%), Basque (39%), Croatian (38%), Czech (37%), Danish (39%), Greek (37%), Norwegian (44%), Persian (39%), Serbian (38%), Sinhala (37%)
- 🟡 **Medium Priority** (50-90% complete): Bulgarian (67%), Finnish (72%), Slovak (72%), Romanian (79%), Slovenian (80%), Hebrew (80%), Galician (80%), Catalan (81%), German (82%), ChineseTraditional (83%), Dutch (86%), Tamil (88%)
- 🟢 **Minor Updates** (90-99% complete): Spanish (97%), Swedish (98%), Japanese (99%), Korean (99%), French (99%), Hungarian (99%), Polish (99%), Portuguese (99%), Turkish (99%), Russian (99%)
- ✅ **Complete** (100%): Brazilian, ChineseSimplified, Corsican, English, Italian, Lithuanian

## 🚀 Quick Start

### Option 1: Update an Existing Translation

1. **Choose a language** you speak fluently from the list above
2. **Create a branch**:
   ```powershell
   git checkout -b update/language-name-translation
   # Example: git checkout -b update/german-translation
   ```

3. **Edit the PO file**:
   - Navigate to `Translations/WinMerge/YourLanguage.po`
   - You can use:
     - **Poedit** (recommended) - Download from https://poedit.net/
     - **VS Code** with gettext extension
     - Any text editor

4. **Find untranslated strings**:
   - Look for entries with `msgid` but empty `msgstr`
   - Example:
     ```po
     msgid "Copy to Middle"
     msgstr ""  # <-- This needs translation
     ```

5. **Add your translation**:
   ```po
     msgid "Copy to Middle"
     msgstr "In die Mitte kopieren"  # German translation
     ```

6. **Update the header**:
   - Update `PO-Revision-Date` to current date
   - Update `Last-Translator` with your name and email

7. **Commit and push**:
   ```powershell
   git add Translations/WinMerge/YourLanguage.po
   git commit -m "Update YourLanguage translation: translate missing strings"
   git push origin update/language-name-translation
   ```

8. **Create Pull Request** on GitHub

### Option 2: Start a New Translation

If your language isn't available:

1. **Copy the English template**:
   ```powershell
   Copy-Item Translations/WinMerge/English.pot Translations/WinMerge/YourLanguage.po
   ```

2. **Edit the header** with your language information
3. **Start translating** `msgstr` fields
4. Follow steps 6-8 from Option 1

## 📝 Translation Guidelines

### Best Practices

1. **Consistency**: Use the same terms throughout
2. **Context**: Understand the context before translating
3. **Keyboard shortcuts**: Keep shortcuts like `\tCtrl+G` unchanged
   ```po
   msgid "&Go to...\tCtrl+G"
   msgstr "&Ir a...\tCtrl+G"  # Keep \tCtrl+G as-is
   ```

4. **Ampersands (&)**: These create keyboard accelerators
   ```po
   msgid "&File"
   msgstr "&Archivo"  # The letter after & becomes the hotkey
   ```

5. **Placeholders**: Keep %s, %d, %1, etc. exactly as they appear
   ```po
   msgid "File %s not found"
   msgstr "Archivo %s no encontrado"  # Keep %s
   ```

6. **Newlines**: Preserve `\n` in strings
   ```po
   msgid "Line 1\nLine 2"
   msgstr "Línea 1\nLínea 2"  # Keep \n
   ```

### Using Poedit (Recommended)

1. **Download**: https://poedit.net/
2. **Open**: Your language's .po file
3. **Translate**: Click on untranslated items and add translations
4. **Save**: Poedit automatically formats and validates
5. **Done**: Ready to commit!

### Manual Editing

If editing manually, ensure:
- UTF-8 encoding
- No syntax errors in PO format
- Proper escaping of quotes: `\"` instead of `"`

## 🔍 Testing Your Translation

### Build WinMerge

```powershell
# Download dependencies (first time only)
.\DownloadDeps.cmd

# Build
.\BuildAll.vs2022.cmd x64
```

### Test Your Language

1. Run `Build\X64\Release\WinMergeU.exe`
2. Go to **Edit → Preferences → General**
3. Select your language
4. Restart WinMerge
5. Verify your translations appear correctly

## 📂 File Structure

```
Translations/
├── WinMerge/           # Main application translations
│   ├── English.pot     # Template file (DO NOT translate)
│   ├── German.po       # German translation
│   ├── Spanish.po      # Spanish translation
│   └── ...             # Other languages
├── ShellExtension/     # Windows Explorer integration
│   └── ...po files
├── InnoSetup/          # Installer translations
│   └── ...isl files
└── TranslationsStatus.md  # Current status
```

## 🌍 Which File to Edit?

- **Main application**: `Translations/WinMerge/YourLanguage.po`
- **Shell extension** (right-click menu): `Translations/ShellExtension/YourLanguage.po`
- **Installer**: `Translations/InnoSetup/YourLanguage.isl`

Most contributions focus on the main application (WinMerge/*.po).

## ✅ Checklist Before Submitting

- [ ] All strings translated (or as many as possible)
- [ ] No syntax errors
- [ ] Keyboard shortcuts preserved
- [ ] Placeholders (%s, %d) preserved
- [ ] Tested in WinMerge (if possible)
- [ ] Header updated with your name and date
- [ ] Commit message is clear
- [ ] Pull request describes changes

## 📋 Example Commit Messages

Good examples:
```
Update German translation: translate 50 missing strings
Complete Ukrainian translation
Fix typos in French translation
Update Brazilian Portuguese translation for v2.16
```

## 🆘 Need Help?

- **Translation questions**: Comment in the GitHub issue
- **Technical issues**: Check the main README.md
- **PO file format**: https://www.gnu.org/software/gettext/manual/html_node/PO-Files.html
- **Contact**: Open an issue at https://github.com/WinMerge/winmerge/issues

## 🏆 Recent Contributors

Check [TranslationsStatus.md](TranslationsStatus.md) for the full list of amazing translators!

## 🎯 Quick Example

Here's a complete example of updating a translation:

```po
# Before
msgid "Copy to Middle"
msgstr ""

# After (German)
msgid "Copy to Middle"
msgstr "In die Mitte kopieren"
```

That's it! You've improved WinMerge for German users! 🎉

---

**Thank you for contributing to WinMerge! Your work helps users worldwide! 🌏**
