#include<iostream>
#include<algorithm>

using namespace std;

int exgcd(int a, int b, int& x, int& y)
{
	if (!b)
	{
		x = 1, y = 0;
		return a;
	}
	int d = exgcd(b, a % b, y, x);
	y -= a / b * x;
	return d;
}

int main()
{
	int n;
	scanf_s("%d", &n);

	while (n--)
	{
		int a, b;
		scanf_s("%d%d", &a, &b);
		int x, y;
		exgcd(a, b, x, y);
		printf("%d %d\n", x, y);
	}

	return 0;
}




