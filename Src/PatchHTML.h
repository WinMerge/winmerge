#define OUTPUT_HTML (OUTPUT_SDIFF + 1)

void print_html_header ();
void print_html_terminator ();
void print_html_diff_header (struct file_data inf[]);
void print_html_diff_terminator ();
void print_html_script (struct change *script);
