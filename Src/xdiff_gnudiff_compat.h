#pragma once

class DiffutilsOptions;
unsigned long make_xdl_flags(const DiffutilsOptions& options);
struct change * diff_2_files_xdiff(struct file_data filevec[], int* bin_status, int bMoved_blocks_flag, unsigned xdl_flags);
