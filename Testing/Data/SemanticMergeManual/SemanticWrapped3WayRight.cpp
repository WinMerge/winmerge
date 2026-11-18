void prepare();
void finish();
void log(const char*);
extern bool debug;

void run()
{
    prepare();
    if (debug)
    {
        log("x");
    }
    finish();
}
