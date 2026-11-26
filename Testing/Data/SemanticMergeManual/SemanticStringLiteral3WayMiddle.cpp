namespace Demo {

const char* BuildStatusMessage(int value)
{
	if (value > 0)
		return "stable version";
	return "fallback";
}

}
