import sys
import os

if len(sys.argv) != 3:
    print("Invalid amount of argument")
    exit(-1)

output_path = sys.argv[1]
amount = int(sys.argv[2])

content = "#pragma once\n\n"
content += "#include <cstddef>\n"
content += "#include <tuple>\n"
content += "#include <type_traits>\n\n\n"
content += "namespace fp::utils {\n"

content += "\tnamespace __internals {\n"

content += "\t\ttemplate <std::size_t I>\n"
content += "\t\tstruct AggregateTag : AggregateTag<I - 1> {};\n\n"
content += "\t\ttemplate <>\n"
content += "\t\tstruct AggregateTag<0> {};\n\n\n"

content += "\t\tstruct AggregateInit {\n"
content += "\t\t\ttemplate <typename T>\n"
content += "\t\t\toperator T();\n"
content += "\t\t};\n\n\n"


content += "\t\ttemplate <typename T>\n"
content += "\t\tstruct AggregateToVoid {\n"
content += "\t\t\tusing Type = void;\n"
content += "\t\t};\n\n\n"


content += "\t\ttemplate <typename T, typename Enable = void, typename ...Args>\n"
content += "\t\tstruct IsAggregateConstructible : std::false_type {};\n\n"

content += "\t\ttemplate <typename T, typename ...Args>\n"
content += "\t\tstruct IsAggregateConstructible<T, typename AggregateToVoid<decltype(T{std::declval<Args> ()...})>::Type, Args...> : std::true_type {};\n\n\n"

content += "\t\ttemplate <typename T, typename ...Args>\n"
content += "\t\tconsteval auto aggregateSize() noexcept -> std::size_t {\n"
content += "\t\t\tif constexpr (IsAggregateConstructible<T, void, Args..., AggregateInit>::value)\n"
content += "\t\t\t\treturn aggregateSize<T, Args..., AggregateInit> ();\n"
content += "\t\t\telse\n"
content += "\t\t\t\treturn sizeof...(Args);\n"
content += "\t\t};\n"

content += "\t} // namespace __internals\n\n"

content += "\ttemplate <typename T>\n"
content += "\trequires (std::is_aggregate_v<T>)\n"
content += "\tstruct AggregateMembersCount {\n"
content += "\t\tstatic constexpr std::size_t value {__internals::aggregateSize<T> ()};\n"
content += "\t};\n\n\n"

content += "\tnamespace __internals {\n"

content += "\t\ttemplate <typename T>\n"
content += "\t\tconsteval auto aggregateMakeTuple(const T &value) {\n"
for val in range(amount, 0, -1):
    args = ",".join([f"m{i}" for i in range(0, val)])
    content += f"\t\t\tif constexpr (AggregateMembersCount<T>::value == std::size_t{{{val}}}) {{\n"
    content += f"\t\t\t\tconst auto &[{args}] {{value}};\n"
    content += f"\t\t\t\treturn std::make_tuple({args});\n"
    content += "\t\t\t};\n"

content += "\t\t}\n\n\n"

content += "\t\ttemplate <typename T>\n"
content += "\t\tconstexpr auto aggregateMakeTie(const T &value) {\n"
for val in range(amount, 0, -1):
    args = ",".join([f"m{i}" for i in range(0, val)])
    content += f"\t\t\tif constexpr (AggregateMembersCount<T>::value == std::size_t{{{val}}}) {{\n"
    content += f"\t\t\t\tconst auto &[{args}] {{value}};\n"
    content += f"\t\t\t\treturn std::tie({args});\n"
    content += "\t\t\t};\n"

content += "\t\t}\n\n\n"

content += "\t\ttemplate <typename T>\n"
content += "\t\tconstexpr auto aggregateMakeTie(T &value) {\n"
for val in range(amount, 0, -1):
    args = ",".join([f"m{i}" for i in range(0, val)])
    content += f"\t\t\tif constexpr (AggregateMembersCount<T>::value == std::size_t{{{val}}}) {{\n"
    content += f"\t\t\t\tauto &[{args}] {{value}};\n"
    content += f"\t\t\t\treturn std::tie({args});\n"
    content += "\t\t\t};\n"

content += "\t\t}\n\n"

content += "\t}; // namespace __internals\n\n"

content += "\ttemplate <typename T>\n"
content += "\trequires (std::is_aggregate_v<T>)\n"
content += "\tstruct AggregateMembers {\n"
content += "\t\tusing Type = decltype(__internals::aggregateMakeTuple<T> (std::declval<T> ()));\n"
content += "\t};\n"


content += "} // namespace fp::utils\n"


#print(content)
os.makedirs(os.path.dirname(output_path), exist_ok=True)
with open(output_path, "w") as file:
    file.write(content)
