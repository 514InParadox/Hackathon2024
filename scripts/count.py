# 数所有字符串大小

def main():
    cnt = 0
    base = 9 # max index le 8，8 or 9?
    with open('keys.txt', 'r') as file:
        line = file.readline()
        while line:
            print(line)
            line = file.readline()
            if line.find(':') != -1:
                if line.find(r'{i}') != -1:
                    if line.find(r'{j}') != -1:
                        cnt += base ** 2
                    cnt += base 
                cnt += 1
    print(cnt)


if __name__ == "__main__":
    main()