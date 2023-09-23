#pragma once

class DiffutilsOptions;

unsigned long make_xdl_flags(const DiffutilsOptions& options);
struct change* diff_2_buffers_xdiff(const char* ptr1, size_t size1, const char* ptr2, size_t size2, unsigned xdl_flags);
struct change * diff_2_files_xdiff(struct file_data filevec[], int* bin_status, int bMoved_blocks_flag, int* bin_file, unsigned xdl_flags);
