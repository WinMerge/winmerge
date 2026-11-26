namespace Demo {

const char* BuildStatusMessage(int value)
{
	if (value > 0)
		return "changed version";
	return "fallback";
}

}
