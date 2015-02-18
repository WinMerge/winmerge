#pragma once

struct file_data;
struct change;

void print_html_header ();
void print_html_terminator ();
void print_html_diff_header (struct file_data inf[]);
void print_html_diff_terminator ();
void print_html_script (struct change *script);
