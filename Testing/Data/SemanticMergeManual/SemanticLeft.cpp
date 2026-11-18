namespace Demo {

int Multiply(int left, int right)
{
	return left * right;
}

int ComputeScore(int base)
{
	int doubled = base * 2;
	return doubled + Multiply(base, 3);
}

const char* BuildMessage()
{
	return "left version";
}

}
