import sys
import os

if len(sys.argv) != 3:
    print("Invalid amount of argument")
    exit(-1)

output_path = sys.argv[1]
amount = int(sys.argv[2])

content = "#pragma once\n\n"

for val in range(0, amount+1):
    args = ",".join([f"e{i}" for i in range(0, val+1)])
    content += f"#define FP_VAR_GET_{val}({args}, ...) e{val}\n"

content += "\n"
content += f"#define FP_VAR_SIZE(...) FP_VAR_GET_{amount}(__VA_ARGS__, {",".join([str(i) for i in range(amount, 0, -1)])})\n"

content += "\n"
content += "/*\n"
content += " * This macro convert a variadic macro argument to a macro sequence.\n"
content += " * This implementation is auto-generated by a python script. Please edit `FP_VAR_TO_SEQ_MAX_SIZE`\n"
content += " * in the root CMakeLists.txt and recompile fipper to increase the maximum size of the sequence.\n"
content += " * \n"
content += " * This implementation is inspired by [Boost.PP](https://www.boost.org/doc/libs/1_87_0/libs/preprocessor/doc/index.html)\n"
content += " * */\n"
content += "#define FP_VAR_TO_SEQ(...) FP_CONCAT(FP_VAR_TO_SEQ_, FP_VAR_SIZE(__VA_ARGS__))(__VA_ARGS__)\n\n"

for val in range(1, amount+1):
    args = ",".join([f"e{i}" for i in range(0, val)])
    values = "".join([f"(e{i})" for i in range(0, val)])
    content += f"#define FP_VAR_TO_SEQ_{val}({args}) {values}\n"

os.makedirs(os.path.dirname(output_path), exist_ok=True)
with open(output_path, "w") as file:
    file.write(content)
