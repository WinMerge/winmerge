# Dockerfile — Reproducible golden baseline capture for WinMerge migration.
#
# Produces golden-baseline.json using GNU diff in a clean, deterministic environment.
#
# Build & run:
#   docker build -t winmerge-baseline migration-reports/pre-migration/baseline/diff-equivalence/
#   docker run --rm -v "$(pwd)":/repo winmerge-baseline
#
# The golden-baseline.json is written back to the bind-mounted repo.

FROM python:3.12-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
    diffutils \
    bash \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /repo

ENTRYPOINT ["bash", "migration-reports/pre-migration/baseline/diff-equivalence/capture-golden-baseline.sh"]
