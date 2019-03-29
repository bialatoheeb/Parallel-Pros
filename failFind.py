def main():
    minvals = [10000 for i in range(99)]
    with open("output.txt") as out:
        lines = out.readlines()
        for linen in lines:
            line = linen.split(",")
            if len(line) >= 2:
                if (len(line[0].split(":")) == 2 and len(line[1].split(":")) ==2 ):
                    rank = int(line[0].split(":")[1])
                    count = int(line[1].split(":")[1])
                    if (minvals[rank] > count):
                        minvals[rank] = count

    for i in range(len(minvals)):
        print("minvals[",i,"]:",minvals[i])


main()
