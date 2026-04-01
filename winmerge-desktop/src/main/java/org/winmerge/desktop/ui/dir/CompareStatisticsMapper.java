package org.winmerge.desktop.ui.dir;

import java.util.Objects;

import org.winmerge.desktop.ui.dialogs.CompareStatisticsDialogModel;

final class CompareStatisticsMapper {
    private CompareStatisticsMapper() {
    }

    static CompareStatisticsDialogModel fromModel(DirDocModel model) {
        Objects.requireNonNull(model, "model");

        long onlyLeft = model.countByStatus(DirDocModel.DirStatus.ONLY_LEFT);
        long onlyRight = model.countByStatus(DirDocModel.DirStatus.ONLY_RIGHT);
        long different = model.countByStatus(DirDocModel.DirStatus.DIFFERENT);
        long identical = model.countByStatus(DirDocModel.DirStatus.IDENTICAL);
        long total = onlyLeft + onlyRight + different + identical;

        return new CompareStatisticsDialogModel(
            model.leftRoot().toString(),
            model.rightRoot().toString(),
            onlyLeft,
            onlyRight,
            different,
            identical,
            total
        );
    }
}
