namespace Demo {

const char* BuildMessage()
{
	return "right version";
}

int ComputeScore(int base)
{
	int doubled = base * 2;
	return doubled + Multiply(base + 1, 3);
}

int Multiply(int left, int right)
{
	return left * right;
}

}
