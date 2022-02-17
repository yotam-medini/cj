#include <iostream>
#include <string>
#include <cstring>

using namespace std;

string GetRuler(const string& kingdom) {
  // TODO: implement this method to determine the ruler name, given the kingdom.
  string ruler = "nobody";
  size_t sz = kingdom.size();
  char clast = kingdom[sz - 1];
  bool vowel = strchr("AEIOUaeiou", clast) != nullptr;
  if (vowel)
  {
      ruler = "Alice";
  }
  else if ((clast != 'y') && (clast != 'Y'))
  {
      ruler = "Bob";
  }
  return ruler;
}

int main() {
  int testcases;
  cin >> testcases;
  string kingdom;

  for (int t = 1; t <= testcases; ++t) {
    cin >> kingdom;
    cout << "Case #" << t << ": " << kingdom << " is ruled by "
         << GetRuler(kingdom) << ".\n";
  }
  return 0;
}
