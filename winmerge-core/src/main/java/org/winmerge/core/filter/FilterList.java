package org.winmerge.core.filter;

import java.util.ArrayList;
import java.util.List;
import java.util.regex.Pattern;
import java.util.regex.PatternSyntaxException;

public final class FilterList {
    private record FilterItem(String filterAsString, Pattern regexp) {
    }

    private final List<FilterItem> list;

    public FilterList() {
        list = new ArrayList<>();
    }

    public void addRegExp(String regularExpression) {
        addRegExp(regularExpression, false);
    }

    public void addRegExp(String regularExpression, boolean throwIfInvalid) {
        try {
            list.add(new FilterItem(regularExpression, Pattern.compile(regularExpression, Pattern.UNICODE_CASE)));
        } catch (PatternSyntaxException exception) {
            if (throwIfInvalid) {
                throw exception;
            }
        }
    }

    public void removeAllFilters() {
        list.clear();
    }

    public boolean hasRegExps() {
        return !list.isEmpty();
    }

    public boolean match(String value) {
        return match(value, FileCodePage.UTF8.codepage());
    }

    public boolean match(String value, int codepage) {
        if (list.isEmpty()) {
            return false;
        }
        for (FilterItem item : list) {
            if (item.regexp.matcher(value).find()) {
                return true;
            }
        }
        return false;
    }

    public void cloneFrom(FilterList filterList) {
        if (filterList == null) {
            return;
        }
        list.clear();
        for (FilterItem item : filterList.list) {
            list.add(new FilterItem(item.filterAsString, Pattern.compile(item.filterAsString, item.regexp.flags())));
        }
    }

    private enum FileCodePage {
        UTF8(65001);

        private final int codepage;

        FileCodePage(int codepage) {
            this.codepage = codepage;
        }

        int codepage() {
            return codepage;
        }
    }
}
