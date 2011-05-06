#ifndef TINKERCELL_GLOBALSETTINGS_H
#define TINKERCELL_GLOBALSETTINGS_H

#include <QString>
#include <QStringList>

#ifdef Q_WS_WIN
#define TINKERCELLEXPORT __declspec(dllexport)
#else
#define TINKERCELLEXPORT
#endif

namespace Tinkercell
{
	/*! \brief 
	This class stores global variables such as project names, enables/disabled feature, etc.
	Use the following static bools to enable or disable features:
	ENABLE_HISTORY_WINDOW
	ENABLE_CONSOLE_WINDOW
	ENABLE_GRAPHING_TOOLS
	ENABLE_CODING_TOOLS
	ENABLE_ALIGNMENT_TOOL
	ENABLE_PYTHON
	ENABLE_RUBY
	ENABLE_OCTAVE
	ENABLE_LOADSAVE_TOOL
	\ingroup core
	*/
	class TINKERCELLEXPORT GlobalSettings
	{
	public:
		/*!\brief enable history window -- defaults to true*/
		static bool ENABLE_HISTORY_WINDOW;
		
		/*!\brief enable console window -- defaults to true*/
		static bool ENABLE_CONSOLE_WINDOW;
		
		/*!\brief enable plot2d, plot3d, and gnuplot -- defaults to false*/
		static bool ENABLE_GRAPHING_TOOLS;
		
		/*!\brief enable coding window and interpreters -- defaults to false*/
		static bool ENABLE_CODING_TOOLS;
		
		/*!\brief enable alignment and other basic GUI -- defaults to true*/
		static bool ENABLE_ALIGNMENT_TOOL;
		
		/*!\brief enable python interpreter -- defaults to false*/
		static bool ENABLE_PYTHON;
		
		/*!\brief enable octave interpreter -- defaults to false*/
		static bool ENABLE_OCTAVE;

		/*!\brief enable octave interpreter -- defaults to false*/
		static bool ENABLE_RUBY;
		
		/*!\brief enable loading and saving -- defaults to true*/
		static bool ENABLE_LOADSAVE_TOOL;

		/*! \brief the project website*/
		static QString PROJECTWEBSITE;
		
		/*! \brief the project organization name*/
		static QString ORGANIZATIONNAME;

		/*! \brief the project name*/
		static QString PROJECTNAME;

		/*! \brief the default function that is loaded in C++ plugins*/
		static QString CPP_ENTRY_FUNCTION;

		/*! \brief the default function that is loaded in C plugins*/
		static QString C_ENTRY_FUNCTION;

		/*! \brief the default project version*/
		static QString PROJECT_VERSION;
		
		/*! \brief an optional string that can be used to change the mode of the application.
			The meaning of this variable depends on the purpose of the application. Empty by default.*/
		static QString PROGRAM_MODE;

		/*! \brief the default file extensions that can be opened*/
		static QStringList OPEN_FILE_EXTENSIONS;

		/*! \brief the default file extensions that can be saved*/
		static QStringList SAVE_FILE_EXTENSIONS;

		/*! \brief register all the TinkerCell data structures with Qt*/
		static void RegisterDataTypes();

		/*!
		* \brief The TinkerCell user directory, which is User's Documents Folder/TinkerCell by default, but users may change this setting
		*/
		static QString homeDir();
		/*!
		* \brief The TinkerCell user temporary directory, which is <SYSTEM TEMP FOLDER>/TinkerCell
		*/
		static QString tempDir();

	};
}

#endif

