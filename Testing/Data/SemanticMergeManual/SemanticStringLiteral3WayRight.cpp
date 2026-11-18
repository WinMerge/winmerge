namespace Demo {

const char* BuildStatusMessage(int value)
{
	const bool hasValue = value > 0;
	if (hasValue)
		return "stable version";
	return "fallback";
}

}
