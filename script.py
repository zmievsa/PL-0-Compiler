def strip(s):
    return s.replace("\t", "").replace(" ", "").lower()

original = """
"""

mine = """
"""

for line1, line2 in zip(strip(original).splitlines(), strip(mine).splitlines()):
    if line1 != line2:
        print(line1, ":::", line2)
    else:
        print(line1, ":::", line2)
        print('\033[93m')

# for line in mine.splitlines():
#     if "CAL" in line:
#         print(line)