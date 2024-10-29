#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cctype>
#include <cstdarg>
#include <cmath>
#include <cstdint>

#define GAR_ABC
#include <ctk-0.5/ctk.cpp>

void panic(const char* format, ...) {
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	exit(1);
}

char* get_arg(const char* name, ar<char*> args) {
	size_t name_len = strlen(name);
	for (size_t a = 0; a < args.len; ++a) {
		char* arg = args[a];
		size_t arg_len = strlen(arg);
		if (arg_len >= 2 + name_len + 2) {
			for (size_t b = 0; b < name_len; ++b) {
				if (arg[2 + b] != name[b]) {
					goto next_arg;
				}
			}
			return arg + 2 + name_len + 1;
		}
		next_arg:
		continue;
	}
	panic("arg '%s' not found", name);
	return nullptr;
}

gar<char> load_file(const char* file_path) {
	FILE* file = std::fopen(file_path, "r");
	if (file == nullptr) {
		panic("fopen failed '%s'", file_path);
	}
	std::fseek(file, 0, SEEK_END);
	size_t file_size = std::ftell(file);
	std::fseek(file, 0, SEEK_SET);
	gar<char> data = gar<char>::alloc(file_size + 1);
	data.len = file_size;
	if (std::fread(data.buf, 1, file_size, file) != file_size) {
		panic("fread failed");
	}
	data.push('\0');
	return data;
}

void replace(gar<char>* data, const char* str_a, const char* str_b) {
	size_t str_a_len = strlen(str_a);
	size_t str_b_len = strlen(str_b);
	while (true) {
		char* str_a_ptr = strstr(data->buf, str_a);
		if (str_a_ptr == nullptr) {
			break;
		}
		size_t index = str_a_ptr - data->buf;
		data->remove_many(index, str_a_len);
		data->insert_many(index, str_b, str_b_len);
	}
}

int main(int argc, char** argv) {
	ar<char*> args = ar<char*>(argv + 1, argc - 1);
	const char* app_name = get_arg("name", args);
	const char* app_desc = get_arg("desc", args);
	const char* app_url = get_arg("url", args);
	const char* main_path = get_arg("main", args);
	const char* html_path = get_arg("html", args);
	const char* css_path = get_arg("css", args);
	const char* js_path = get_arg("js", args);
	gar<char> main_data = load_file(main_path);
	gar<char> html_data = load_file(html_path);
	gar<char> css_data = load_file(css_path);
	gar<char> js_data = load_file(js_path);
	replace(&main_data, "%APP_NAME%", app_name);
	replace(&main_data, "%APP_DESC%", app_desc);
	replace(&main_data, "%APP_URL%", app_url);
	replace(&main_data, "%INLINE_HTML%", html_data.buf);
	replace(&main_data, "%INLINE_CSS%", css_data.buf);
	replace(&main_data, "%INLINE_JS%", js_data.buf);
	fputs(main_data.buf, stdout);
}