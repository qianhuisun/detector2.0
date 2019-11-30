import os, sys

if __name__ == "__main__":
    path = sys.argv[1]
    output_path = os.path.join(os.path.curdir, "output")
    if not os.path.isdir(output_path):
        os.mkdir(output_path)
    for root, dirs, files in os.walk(path):
        for file in files:
            with open(os.path.join(root, file)) as f:
                with open(os.path.join(output_path, file), 'w') as fout:
                    for line in f.readlines():
                        if len(line) > 0 and line[0] == '{':
                            fout.write('{0}'.format(line))
