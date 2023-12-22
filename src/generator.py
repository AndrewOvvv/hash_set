import random, string

def randomword(length):
   letters = string.ascii_lowercase
   return ''.join(random.choice(letters) for i in range(length))

n = int(input())
s = set()
while len(s) < n:
    st = randomword(30)
    if st not in s:
        s.add(st)
        print(randomword(30))
