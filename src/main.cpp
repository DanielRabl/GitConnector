#include <qpl/qpl.hpp>


void execute_batch(const std::string& path, const std::string& data) {
	qpl::filesys::create_file(path, data);
	std::system(path.c_str());
	qpl::filesys::remove(path);
}

int main() try {
	auto home_path = qpl::filesys::get_current_location();
	std::string github = "https://github.com/DanielRabl";

	auto list = home_path.list_current_directory();
	list.list_remove_files();
	list.print_tree();
	qpl::println();

	qpl::filesys::path target;
	while (true) {
		qpl::print("select directory > ");
		auto directory = qpl::get_input();

		list = home_path.list_current_directory();
		for (auto& path : list) {
			if (qpl::string_equals_ignore_case(directory, path.get_directory_name())) {
				target = path.ensured_directory_backslash();
				break;
			}
		}
		if (!target.empty()) {
			break;
		}
		qpl::println("couldn't find directory \"", directory, "\"");
	}
	qpl::println("target = ", target);

	auto git_target = target;
	git_target.append("git/");
	git_target.ensure_branches_exist();

	list = git_target.list_current_directory();
	bool already_git_directory = false;
	for (auto& i : list) {
		if (i.is_directory() && i.get_directory_name() == ".git") {
			already_git_directory = true;
			break;
		}
	}
	if (already_git_directory) {
		qpl::println();
		qpl::println(target, " already has git/.git");
		while (true) {
			qpl::print("would you like to reset the folder? (y/n) > ");
			auto input = qpl::get_input();

			if (qpl::string_equals_ignore_case(input, "y")) {
				git_target.remove();
				git_target.ensure_branches_exist();
				break;
			}
			else if (qpl::string_equals_ignore_case(input, "n")) {
				return 0;
			}
		}
	}

	qpl::println();
	std::string github_repos_name;
	while (true) {
		qpl::print("github repository name [ enter to use \"", target.get_directory_name(), "\" ] > ");
		auto input = qpl::get_input();

		if (input.empty()) {
			github_repos_name = target.get_directory_name();
			break;
		}
		else {
			github_repos_name = input;
		}
	}
	auto github_url = qpl::to_string(github, "/", github_repos_name, ".git");

	qpl::println();
	bool push = false;
	while (true) {
		qpl::print("pull or push > ");
		auto input = qpl::get_input();
		if (qpl::string_equals_ignore_case(input, "push")) {
			push = true;
			break;
		}
		else if (qpl::string_equals_ignore_case(input, "pull")) {
			push = false;
			break;
		}
		qpl::println("invalid input \"", input, "\"");
	}

	if (push) {
		auto empty = git_target;
		empty.append("empty");
		empty.create();
	}

	auto set_directory = qpl::to_string("cd ", git_target);
	auto batch = home_path.appended("git_connector.bat");
	auto batch_data = qpl::to_string(set_directory, " && git init && git remote add origin ", github_url, " && git branch -M main");

	if (push) {
		batch_data.append(" && git add -A && git commit -m \"first init\" && git push -u origin main");
	}
	else {
		batch_data.append(" && git pull --set-upstream origin main");
	}
	qpl::println();
	qpl::println("execute = ", batch_data);
	execute_batch(batch, batch_data);

	if (push) {
		auto empty = git_target;
		empty.append("empty");
		empty.remove();
	}
}
catch (std::exception& any) {
	qpl::println("caught exception:\n", any.what());
	qpl::system_pause();
}