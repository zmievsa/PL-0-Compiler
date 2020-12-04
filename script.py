with open("error_outputs.txt") as f:
    text = f.read()

i = 1
while "Error on line" in text:
    text = text.replace("Error on line", f"Error number {i} on line", 1)
    i += 1
print(text)
with open("error_outputs.txt", "w") as f:
    f.write(text)