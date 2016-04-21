#include "synctool.h"

/* Command line options */
bool gUseColors = true;
bool gFastMode = false;
bool gVerbose = false;
string gSyncMode = "";
list<string> gBlacklist;

/* Handles command line arguments*/
template <typename T>
inline bool handle_arg(string arg, string name, string shortname, T& var, T yes)
{
	if (arg == string(string("--") + name) || arg == string(string("-") + name) || arg == string(string("-") + shortname))
	{
		var = yes; return true;
	}
	return false;
}

template <typename T>
inline bool handle_arg(string arg, string name, string shortname, T& var, T yes, T no)
{
	if (arg == string(string("--") + name) || arg == string(string("-") + name) || arg == string(string("-") + shortname))
	{
		var = yes; return true;
	}
	else if (arg == string(string("--no-") + name) || arg == string(string("-no-") + name))
	{
		var = no; return true;
	}
	return false;
}

inline bool handle_arg_with_option(int argc, char *argv[], int index, string name, string shortname, string &option)
{
	string arg = argv[index];
	if (arg == string(string("--") + name) || arg == string(string("-") + name) || arg == string(string("-") + shortname))
	{
		if (index < argc - 1) {
			option = argv[index + 1];
			return true;
		}
	}
	return false;
}

void logMessage(string msg, string color)
{
	setColor(color);
	cout << msg << endl;
}

void die(int code, string msg)
{
	setColor(WHITE);
	cout << endl << endl << msg << endl << endl
		<< "Press enter to exit..." << endl;

	(void)getchar();
	exit(code);
}

void printHelp()
{
	logMessage(
		"SyncTool version " VERSION "\n\n"
		"Usage:\n"
		"# synctool [source] [dest] <options>\n\n"
		"Options:\n"
		" -f : Use a faster (but less reliable) method of comparing files\n"
		" -c : Colorize the program messages\n"
		" -v : Write extra progress messages\n"
		" -i : Run in interactive mode\n"
		" -x <name> : Exclude files containing <name>\n\n"
		"Sync modes:\n"
		" -m : Mirror mode (default)\n"
		" -a : Append mode\n"
	);
	die(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
	bool interactive = false;
	string src = "", dst = "";

	/* Handle command-line arguments */
	for (int i = 1; i < argc; i++)
	{
		bool isArg = false;
		string argOption;

		isArg |= handle_arg_with_option(argc, argv, i, "exclude", "x", argOption);
		if (isArg) {
			gBlacklist.push_back(argOption);

			cout << "Excluding files containing " << argOption << endl;
			i++;
			continue;
		}

		isArg |= handle_arg(argv[i], "color",       "c", gUseColors,  true, false);
		isArg |= handle_arg(argv[i], "fast",        "f", gFastMode,   true, false);
		isArg |= handle_arg(argv[i], "verbose",     "v", gVerbose,    true, false);
		isArg |= handle_arg(argv[i], "interactive", "i", interactive, true, false);

		isArg |= handle_arg<string>(argv[i], "append", "a", gSyncMode, "append");
		isArg |= handle_arg<string>(argv[i], "mirror", "m", gSyncMode, "mirror");

		if (!isArg && ((!isDirectory(argv[i])) || ((src != "") && (dst != ""))))
			logMessage("Warning: Unknown option: " + string(argv[i]));
		else if (!isArg && src == "")
			src = argv[i];
		else if (!isArg && dst == "")
			dst = argv[i];
	}

	if (interactive)
	{
		logMessage("Entering interactive mode...");

		if (src == "")
		{
			logMessage("Enter source directory: ");
			cin >> src;
		}

		if (dst == "")
		{
			logMessage("Enter destination directory: ");
			cin >> dst;
		}

		while (gSyncMode != "mirror" && gSyncMode != "append")
		{
			logMessage("Choose sync mode [mirror/append]: ");
			cin >> gSyncMode;
		}
	}
	else if (gSyncMode == "")
	{
		gSyncMode = "mirror";
	}

	if (src == "" || dst == "")
	{
		logMessage("Error: Source and destination not specified");
		printHelp();
	}

	if (src == dst)
		die(EXIT_FAILURE, "Error: Source and destination can't be the same");

	assertCanOpenDirectory(src);
	assertCanOpenDirectory(dst);
	doSync(src, dst);

	die(EXIT_SUCCESS, "Done!");
}
