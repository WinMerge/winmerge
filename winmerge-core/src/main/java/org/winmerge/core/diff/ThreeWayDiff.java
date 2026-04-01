package org.winmerge.core.diff;

import java.util.List;
import java.util.function.Predicate;

public final class ThreeWayDiff {
    private ThreeWayDiff() {
    }

    public static int make3WayDiff(
            List<DiffRange> diff3,
            List<DiffRange> diff10,
            List<DiffRange> diff12,
            Predicate<DiffRange> compare02,
            boolean hasTrivialDiffs) {
        int diff10Count = diff10.size();
        int diff12Count = diff12.size();

        int diff10i = 0;
        int diff12i = 0;

        int lineLast0 = 0;
        int lineLast1 = 0;
        int lineLast2 = 0;

        while (true) {
            if (diff10i >= diff10Count && diff12i >= diff12Count) {
                break;
            }

            DiffRange dr3 = new DiffRange();
            DiffRange dr10First = null;
            DiffRange dr10Last = null;
            DiffRange dr12First = null;
            DiffRange dr12Last = null;

            boolean firstDiffBlockIsDiff12;
            if (diff10i >= diff10Count) {
                dr12First = copy(diff12.get(diff12i));
                dr12Last = copy(dr12First);
                firstDiffBlockIsDiff12 = true;
            } else if (diff12i >= diff12Count) {
                dr10First = copy(diff10.get(diff10i));
                dr10Last = copy(dr10First);
                firstDiffBlockIsDiff12 = false;
            } else {
                dr10First = copy(diff10.get(diff10i));
                dr10Last = copy(dr10First);
                dr12First = copy(diff12.get(diff12i));
                dr12Last = copy(dr12First);
                firstDiffBlockIsDiff12 = dr12First.begin()[0] <= dr10First.begin()[0];
            }

            boolean lastDiffBlockIsDiff12 = firstDiffBlockIsDiff12;
            int diff10itmp = diff10i;
            int diff12itmp = diff12i;

            while (true) {
                if (diff10itmp >= diff10Count || diff12itmp >= diff12Count) {
                    break;
                }

                DiffRange dr10 = copy(diff10.get(diff10itmp));
                DiffRange dr12 = copy(diff12.get(diff12itmp));

                if (dr10.end()[0] == dr12.end()[0]) {
                    diff10itmp++;
                    lastDiffBlockIsDiff12 = true;
                    dr10Last = dr10;
                    dr12Last = dr12;
                    break;
                }

                if (lastDiffBlockIsDiff12) {
                    if (Math.max(dr12.begin()[0], dr12.end()[0]) < dr10.begin()[0]) {
                        break;
                    }
                } else if (Math.max(dr10.begin()[0], dr10.end()[0]) < dr12.begin()[0]) {
                    break;
                }

                if (dr12.end()[0] > dr10.end()[0]) {
                    diff10itmp++;
                    lastDiffBlockIsDiff12 = true;
                } else {
                    diff12itmp++;
                    lastDiffBlockIsDiff12 = false;
                }

                dr10Last = dr10;
                dr12Last = dr12;
            }

            if (lastDiffBlockIsDiff12) {
                diff12itmp++;
            } else {
                diff10itmp++;
            }

            if (firstDiffBlockIsDiff12) {
                dr3.begin()[1] = dr12First.begin()[0];
                dr3.begin()[2] = dr12First.begin()[1];
                if (diff10itmp == diff10i) {
                    dr3.begin()[0] = dr3.begin()[1] - lineLast1 + lineLast0;
                } else {
                    dr3.begin()[0] = dr3.begin()[1] - dr10First.begin()[0] + dr10First.begin()[1];
                }
            } else {
                dr3.begin()[0] = dr10First.begin()[1];
                dr3.begin()[1] = dr10First.begin()[0];
                if (diff12itmp == diff12i) {
                    dr3.begin()[2] = dr3.begin()[1] - lineLast1 + lineLast2;
                } else {
                    dr3.begin()[2] = dr3.begin()[1] - dr12First.begin()[0] + dr12First.begin()[1];
                }
            }

            if (lastDiffBlockIsDiff12) {
                dr3.end()[1] = dr12Last.end()[0];
                dr3.end()[2] = dr12Last.end()[1];
                if (diff10itmp == diff10i) {
                    dr3.end()[0] = dr3.end()[1] - lineLast1 + lineLast0;
                } else {
                    dr3.end()[0] = dr3.end()[1] - dr10Last.end()[0] + dr10Last.end()[1];
                }
            } else {
                dr3.end()[0] = dr10Last.end()[1];
                dr3.end()[1] = dr10Last.end()[0];
                if (diff12itmp == diff12i) {
                    dr3.end()[2] = dr3.end()[1] - lineLast1 + lineLast2;
                } else {
                    dr3.end()[2] = dr3.end()[1] - dr12Last.end()[0] + dr12Last.end()[1];
                }
            }

            lineLast0 = dr3.end()[0] + 1;
            lineLast1 = dr3.end()[1] + 1;
            lineLast2 = dr3.end()[2] + 1;

            if (diff10i == diff10itmp) {
                dr3.setOperation(OperationType.THIRD_ONLY);
            } else if (diff12i == diff12itmp) {
                dr3.setOperation(OperationType.FIRST_ONLY);
            } else if (!compare02.test(dr3)) {
                dr3.setOperation(OperationType.DIFF);
            } else {
                dr3.setOperation(OperationType.SECOND_ONLY);
            }

            if (hasTrivialDiffs) {
                boolean trivialDiff10 = true;
                boolean trivialDiff12 = true;
                for (int i = diff10i; i < diff10itmp; i++) {
                    if (diff10.get(i).operation() != OperationType.TRIVIAL) {
                        trivialDiff10 = false;
                        break;
                    }
                }
                for (int i = diff12i; i < diff12itmp; i++) {
                    if (diff12.get(i).operation() != OperationType.TRIVIAL) {
                        trivialDiff12 = false;
                        break;
                    }
                }
                if (trivialDiff10 && trivialDiff12) {
                    dr3.setOperation(OperationType.TRIVIAL);
                }
            }

            diff10i = diff10itmp;
            diff12i = diff12itmp;
            diff3.add(dr3);
        }

        int count = diff3.size();
        if (count > 0) {
            for (int i = 0; i < count - 1; i++) {
                DiffRange current = diff3.get(i);
                DiffRange next = diff3.get(i + 1);
                for (int j = 0; j < 3; j++) {
                    if (current.end()[j] >= next.begin()[j]) {
                        current.end()[j] = next.begin()[j] - 1;
                    }
                }
            }
        }
        return count;
    }

    private static DiffRange copy(DiffRange source) {
        return new DiffRange(source);
    }
}
