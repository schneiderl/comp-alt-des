import random

size = 50000
string = ""
for i in range(0,size):
    string = string + " " + str(random.randint(0,1000000))

print (string)