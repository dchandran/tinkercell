/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 The parent class for all text parsers. TextEditor requires at least one
 text parser; otherwise it does no function.
 
****************************************************************************/

#include "TextParser.h"

namespace Tinkercell
{
	TextParser * TextParser::_parser = 0;
	
	void TextParser::parse(TextEditor*)
	{
	}
	
	void TextParser::textChanged(TextEditor *, const QString&, const QString&, const QString&)
	{
	}
    
	void TextParser::lineChanged(TextEditor *, int, const QString&)
	{
	}
	
	void TextParser::deactivate()
	{			
	}
	
	void TextParser::activate()
	{		
		TextParser::setParser(this);
	}
	
	void TextParser::setParser(TextParser* parser)
	{
		if (!parser || !parser->mainWindow) return;
		
		MainWindow * main = parser->mainWindow;
		
		main->disconnect(SIGNAL(parse(TextEditor*)));
		main->disconnect(SIGNAL(textChanged(TextEditor *, const QString&, const QString&, const QString&)));
		main->disconnect(SIGNAL(lineChanged(TextEditor *, int, const QString&)));
		
		connect(main,SIGNAL(parse(TextEditor*)),parser,SLOT(parse(TextEditor*)));
		
		connect(main,SIGNAL(textChanged(TextEditor *, const QString&, const QString&, const QString&)),
				parser,SLOT(textChanged(TextEditor *, const QString&, const QString&, const QString&)));
		
		connect(main,SIGNAL(lineChanged(TextEditor *, int, const QString&)),
				parser,SLOT(lineChanged(TextEditor *, int, const QString&)));
		
		if (TextParser::_parser)
			TextParser::_parser->deactivate();
		
		TextParser::_parser = parser;
	}

	TextParser* TextParser::currentParser()
	{
		return _parser;
	}
	
}
