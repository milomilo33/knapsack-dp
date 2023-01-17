import random

with open("weights.txt", "w") as w:
    for n in range(0, 800-1):
        w.write(str(random.randint(0,10000)))
        w.write("\n")
    w.write(str(random.randint(0,10000)))

with open("values.txt", "w") as w:
    for n in range(0, 800-1):
        w.write(str(random.randint(0,10000)))
        w.write("\n")
    w.write(str(random.randint(0,10000)))