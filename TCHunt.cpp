// ~ #####################################################
// ~ Copyright 2007-2011 16Systems, LLC. All rights reserved.

//~ TCHunt is free software: you can redistribute it and/or modify
//~ it under the terms of the GNU General Public License as published by
//~ the Free Software Foundation, either version 3 of the License, or
//~ (at your option) any later version.

//~ TCHunt is distributed in the hope that it will be useful,
//~ but WITHOUT ANY WARRANTY; without even the implied warranty of
//~ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//~ GNU General Public License for more details.

//~ You should have received a copy of the GNU General Public License
//~ along with TCHunt.  If not, see <http://www.gnu.org/licenses/>.

//~ Email brad@16s.us

//~ 16 Systems, LLC
//~ P.O. Box 356
//~ Blacksburg, VA
//~ 24063 
//~ #####################################################

// Boost Includes
#include "boost/date_time/posix_time/posix_time.hpp"	// ISO Time
#include "boost/date_time/gregorian/gregorian.hpp"	// ISO Date
#include "boost/filesystem/operations.hpp"			// Filesystem Iteration

// Standard Includes
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <numeric>

// FLTK Includes
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/x.H>


// Globals

namespace build
{
	// Set to de, en, fr, es, it, jp, pt or  cn.
	const static std::string lang = "en";
	
	// Set to debug or release.
	const static std::string mode = "release";
  
	// Set to Trial or Full
	// Trial cannot save reports and only searches optical drives.
	const static std::string ver = "Full";
}	

// When releasing builds, make no edits below this point. #######################

namespace lang
{
	const static std::string version_date = "TCHunt 1.5a";
	std::string static copyright;
	std::string static license;
	std::string static yes;
	std::string static end;
	std::string static y;
	std::string static n;
	std::string static save;
	
	// Added to localize report saving GUI
	std::string static all;
	std::string static add_favorites;
	std::string static favorites;
	std::string static preview;
	std::string static ok;
	std::string static cancel;
	std::string static show;
	std::string static custom;
	std::string static my_computer;
	std::string static new_folder;
	std::string static close;
	std::string static manage;
	std::string static select_folder;
	std::string static folder;
}


//###########################################################
// Create output files when in 'debug' mode.

inline void cf( const std::string& mode )
{
  
	if ( mode == "debug" )
	{
		std::ofstream rfile ( "random.txt" );
		rfile.close();	  
	
		std::ofstream mfile ( "modulo.txt" );
		mfile.close();	
	
		std::ofstream hfile ( "header.txt" );
		hfile.close();	
	
		std::ofstream dfile ( "chi.txt" );
		dfile.close();	

		std::ofstream efile ( "exceptions.txt" );
		efile.close();
	
		std::ofstream ofile ( "report.txt" );
		ofile.close();
	}
  
	else
		return;
	
}


//###########################################################
// Skip files that fail the modulo test.

inline bool modulo( const std::string& file_path, const uintmax_t file_size, const std::string& mode )
{
	// File Passed
	if ( file_size % 512 == 0 )
	{
		if ( mode == "debug" )
		{
			std::ofstream mfile ( "modulo.txt", std::ios::app );
			mfile << "true:\t" << file_path << "\t" << file_size << std::endl;
			mfile.close();
			return true;
		}
		else
			return true;
	}
		
	// File Failed
	else
	{
		if ( mode == "debug" )
		{
			std::ofstream mfile ( "modulo.txt", std::ios::app );
			mfile << "false:\t" << file_path << "\t" << file_size << std::endl;
			mfile.close();
			return false;
		}
		else
			return false;
	}
}


//#############################################################
// Skip files that are not made-up of evenly distributed bytes. Chi Squared Distribution.

inline bool X2( const std::string& file_path, const std::string& mode )
{
	// sample_size = bytes read.
	// possibilities = number of possible byte values.
	static const int sample_size = 1024*13;
	static const int possibilities = 256;
	
	double chi = 0.0;
	static const double min = 163.0;
	static const double max = 373.0;
	static const double expected = sample_size/possibilities;
	std::vector<double> observed_frequency;
	
	std::vector<int> sample_bytes;

	std::ifstream fd;
  
	// Open the file in binary mode for reading.
	fd.open (file_path.c_str(), std::ios::binary);
	
	int i = 0;
	if ( fd.is_open() )
	{
		// Read bytes	    
		while ( i < sample_size )
		{
			int c = fd.get();
			sample_bytes.push_back( c );
			++i;
		}
		
		// Close the file
		fd.close();
		
		//std::cout << sample_bytes.size() << std::endl;
	}
	else
	{
		if ( mode == "debug" )
		{
			std::ofstream efile ( "exceptions.txt", std::ios::app );
			efile << "X2:\tcannot open file:\t" << file_path << std::endl;
			efile.close();
			return false;
		}
		else
			return false;
	}
	
	
	for ( int x = 0; x < possibilities; ++x )
	{
		// Populate observed_frequency
		double num_chars = count( sample_bytes.begin(), sample_bytes.end(), x  );
		observed_frequency.push_back(num_chars);
	}
	
	//std::cout << observed_frequency.size() << std::endl;
	
	//~ The four chi squared calculations
	//~ one = observed - expected
	//~ two = one squared
	//~ three = two/expected (this is individual chi)
	//~ four = sum the individual chi results for one X2 score.
	
	std::vector<double>::iterator it;
	for( it = observed_frequency.begin(); it != observed_frequency.end(); ++it )
	{	
		double observed = *( it );
		double one = observed - expected;
		double two = one * one;
		double three = two / expected;
		chi += three;
	}

	if ( chi < min )
	{
		if ( mode == "debug" )
		{
			std::ofstream dfile ( "chi.txt", std::ios::app );
			dfile << "Nandom:\t" << file_path << "\t" << chi << std::endl;
			dfile.close();
			return false;
		}
		else
			return false;
	}
  
	else if ( chi > max )
	{
		if ( mode == "debug" )
		{
			std::ofstream dfile ( "chi.txt", std::ios::app );
			dfile << "Nandom:\t" << file_path << "\t" << chi << std::endl;
			dfile.close();
			return false;
		}
		else
			return false;
	}
  
	else
	{
		if ( mode == "debug" )
		{
			std::ofstream dfile ( "chi.txt", std::ios::app );
			dfile << "Random:\t" << file_path << "\t" << chi << std::endl;
			dfile.close();
			return true;
		}
		else
			return true;
	}  
}


//###########################################################
// Skip Certian files based on the file header (up to 4 bytes). 

inline bool header( const std::string& file_path, const std::string& mode )
{
	int i = 0;

	std::vector<int> header_bytes;

	std::ifstream fd;
  
	// Open the file in binary mode for reading.
	fd.open (file_path.c_str(), std::ios::binary);

	if (fd.is_open())
	{
		// Read First 4 bytes
		while (i < 4)
		{
			header_bytes.push_back(fd.get());
			++i;
		}
		// Close the file
		fd.close();
	}

	else
	{
		if ( mode == "debug" )
		{
			std::ofstream efile ( "exceptions.txt", std::ios::app );
			efile << "header:\tcannot open file:\t" << file_path << std::endl;
			efile.close();
			return false;			
		}
		else
			return false;
	}
	
	int byte1, byte2, byte3, byte4;
	byte1 = header_bytes[0];
	byte2 = header_bytes[1];
	byte3 = header_bytes[2];
	byte4 = header_bytes[3];    
	
	// Skip GZIP files
	//
	// Three Byte Header
	// Hex(1f 8b 08) Dec(31 139 8)
	if ( byte1 == 31 and byte2 == 139 and byte3 == 8 )
	{
		if ( mode == "debug" )
		{
			std::ofstream hfile ( "header.txt", std::ios::app );
			hfile << "header_GZIP:\t" << file_path << std::endl;
			hfile.close();
			return true;
		}
		else
			return true;
	}
	
	
	// Skip RAR files
	//
	// Four Byte Header
	// Hex(52 61 72 21) Dec(82 97 114 33)
	else if ( byte1 == 82 and byte2 == 97 and byte3 == 114 and byte4 == 33 )
	{
		if ( mode == "debug" )
		{
			std::ofstream hfile ( "header.txt", std::ios::app );
			hfile << "header_RAR:\t" << file_path << std::endl;
			hfile.close();
			return true;
		}
		else
			return true;
	}

	// Skip GIF files
	//
	// Three Byte Header
	// Hex(47 49 46 38) Dec(71 73 70 56) GIF8
	else if ( byte1 == 71 and byte2 == 73 and byte3 == 70 and byte4 == 56 )
	{
		if ( mode == "debug" )
		{
			std::ofstream hfile ( "header.txt", std::ios::app );
			hfile << "header_GIF:\t" << file_path << std::endl;
			hfile.close();
			return true;
		}
		else
			return true;
	}
	
	// Skip ZIP files
	// 4 byte header Hex(50 4b 03 04) Dec(80 75 3 4)
	else if ( byte1 == 80 and byte2 == 75 and byte3 == 3 and byte4 == 4 )
	{
		if ( mode == "debug" )
		{
			std::ofstream hfile ( "header.txt", std::ios::app );
			hfile << "header_ZIP:\t" << file_path << std::endl;
			hfile.close();
			return true;
		}
		else
			return true;
	}
	
	// Skip VDF files (Virus Database File from Avira)
	// 4 byte header 'Viru' Hex(56 69 72 75) Dec(86 105 114 117)
	else if ( byte1 == 86 and byte2 == 105 and byte3 == 114 and byte4 == 117 )
	{
		if ( mode == "debug" )
		{
			std::ofstream hfile ( "header.txt", std::ios::app );
			hfile << "header_ZIP:\t" << file_path << std::endl;
			hfile.close();
			return true;
		}
		else
			return true;
	}
	
	// Skip .ar | .deb files
	// 4 byte header Hex(21 3c 61 72) Dec(33 60 97 114)
	// Total header is "!<arch>"
	else if ( byte1 == 33 and byte2 == 60 and byte3 == 97 and byte4 == 114 )
	{
		if ( mode == "debug" )
		{
			std::ofstream hfile ( "header.txt", std::ios::app );
			hfile << "header_AR:\t" << file_path << std::endl;
			hfile.close();
			return true;
		}
		else
			return true;
	}
	
	// Skip .cab files
	// 4 byte header Dec(77 83 67 70)
	else if ( byte1 == 77 and byte2 == 83 and byte3 == 67 and byte4 == 70 )
	{
		if ( mode == "debug" )
		{
			std::ofstream hfile ( "header.txt", std::ios::app );
			hfile << "header_CAB:\t" << file_path << std::endl;
			hfile.close();
			return true;
		}
		else
			return true;
	}
	
	// Skip .png files
	// 4 byte header Dec(137 80 78 71)
	else if ( byte1 == 137 and byte2 == 80 and byte3 == 78 and byte4 == 71 )
	{
		if ( mode == "debug" )
		{
			std::ofstream hfile ( "header.txt", std::ios::app );
			hfile << "header_PNG:\t" << file_path << std::endl;
			hfile.close();
			return true;
		}
		else
			return true;
	}
	
	// Skip .jpg files
	// 4 byte header Dec(255 216 255 224)
	else if ( byte1 == 255 and byte2 == 216 and byte3 == 255 and byte4 == 224 )
	{
		if ( mode == "debug" )
		{
			std::ofstream hfile ( "header.txt", std::ios::app );
			hfile << "header_JPG:\t" << file_path << std::endl;
			hfile.close();
			return true;
		}
		else
			return true;
	}
	
	// Skip .xex (xbox 360) files
	// 4 byte header Dec(88 69 88 50)
	else if ( byte1 == 88 and byte2 == 69 and byte3 == 88 and byte4 == 50 )
	{
		if ( mode == "debug" )
		{
			std::ofstream hfile ( "header.txt", std::ios::app );
			hfile << "header_XEX:\t" << file_path << std::endl;
			hfile.close();
			return true;
		}
		else
			return true;
	}

	// Skip ID3 files
	// Dec(73 68 51 3)
	else if ( byte1 == 73 and byte2 == 68 and byte3 == 51 and byte4 == 3 )
	{
		if ( mode == "debug" )
		{
			std::ofstream hfile ( "header.txt", std::ios::app );
			hfile << "header_ID3:\t" << file_path << std::endl;
			hfile.close();
			return true;
		}
		else
			return true;
	}		
		
	// Skip RIFF
	// Hex(52 49 46 46) Dec(82 73 70 70)
	else if ( byte1 == 82 and byte2 == 73 and byte3 == 70 and byte4 == 70 )
	{
		if ( mode == "debug" )
		{
			std::ofstream hfile ( "header.txt", std::ios::app );
			hfile << "header_RIFF:\t" << file_path << std::endl;
			hfile.close();
			return true;
		}
		else
			return true;
	}
	
	// Skip PDF
	// Hex(25 50 44 46) Dec(37 80 68 70)
	else if ( byte1 == 37 and byte2 == 80 and byte3 == 68 and byte4 == 70 )
	{
		if ( mode == "debug" )
		{
			std::ofstream hfile ( "header.txt", std::ios::app );
			hfile << "header_PDF:\t" << file_path << std::endl;
			hfile.close();
			return true;
		}
		else
			return true;
	}
	
	// Skip BZ2
	// Hex(42 5A 68 39) Dec(66 90 104 57)
	else if ( byte1 == 66 and byte2 == 90 and byte3 == 104 and byte4 == 57 )
	{
		if ( mode == "debug" )
		{
			std::ofstream hfile ( "header.txt", std::ios::app );
			hfile << "header_BZ2:\t" << file_path << std::endl;
			hfile.close();
			return true;
		}
		else
			return true;
	}
	
	// Skip epi
	// Hex(fd 86 a4 c9) Dec(253 134 164 201)
	else if ( byte1 == 253 and byte2 == 134 and byte3 == 164 and byte4 == 201 )
	{
		if ( mode == "debug" )
		{
			std::ofstream hfile ( "header.txt", std::ios::app );
			hfile << "header_EPI:\t" << file_path << std::endl;
			hfile.close();
			return true;
		}
		else
			return true;
	}
	
	// Skip MPQ (World of WarCraft data files)
	// Hex(4d 50 51) Dec(77 80 81)
	else if ( byte1 == 77 and byte2 == 80 and byte3 == 81 )
	{
		if ( mode == "debug" )
		{
			std::ofstream hfile ( "header.txt", std::ios::app );
			hfile << "header_MPQ:\t" << file_path << std::endl;
			hfile.close();
			return true;
		}
		else
			return true;
	}
	
	// File does not have a known file header.
	else
	{
		if ( mode == "debug" )
		{
			std::ofstream hfile ( "header.txt", std::ios::app );
			hfile << "no_header:\t" << file_path << std::endl;
			hfile.close();
			return false;
		}
		else
			return false;
	}
  
}


//###########################################################
// Iterate through the filesystem applying tests.

void files( const boost::filesystem::path& dir_path, const unsigned int tc_min_file_size, int& sfc, const std::string& mode, Fl_Browser * op )
{
  
	std::string str_path;
	boost::filesystem::directory_iterator end_itr;
	
	for ( boost::filesystem::directory_iterator itr(dir_path); itr != end_itr; ++itr )
	{
		str_path = itr->path().string();
		
		Fl::check();
		
		try
		{
			// Folders
			// !symbolic_link_exists(itr->path()) is required in order for this to work on Linux builds. It is not required for Windows.
			if ( is_directory( itr->status() ) and !symbolic_link_exists( itr->path() ) )
			{
				// Recursion
				files( str_path, tc_min_file_size, sfc, mode, op );
			}
		}
    
		catch (...) 
		{
			if ( mode == "debug" )
			{
				std::ofstream efile ( "exceptions.txt", std::ios::app );
				efile << "boost::filesystem directory exception: " << str_path << std::endl; 
				efile.close();
			}
			else
				;
		}
  
		try
		{
			// Regular files 
			// !symbolic_link_exists(itr->path()) is required in order for this to work on Linux builds. It is not required for Windows.
			if ( is_regular( itr->status() ) and !symbolic_link_exists( itr->path() ) )
			{
				// Get file's size in bytes
				const uintmax_t filesize = file_size( itr->path() );
				
				// 1. If the file is above minimum file size and it not the exact same size of oembios.bin, pass it on
				if ( filesize >= tc_min_file_size and filesize != 13107200 )
				{				
					// 2. If the file passess the modulo test, pass it on
					if ( modulo( str_path, filesize, mode ) == true )
					{

						// 3. If file passes X2 Test, pass it on
						if ( X2( str_path, mode ) == true )
						{
							// 4. If file has no known header, stop and report it
							if ( header( str_path, mode ) == false )
							{
								// increment sfc
								++sfc;
								
								// Convert int to std::string
								std::string s;
								std::stringstream count;
								count << sfc;
								s = count.str();

								// Show output to user (add it to browser)
								op->add( ( s + "\t" + str_path + "\n" ).c_str() );
								
								if ( mode == "debug" )
								{
									std::ofstream ofile ( "report.txt", std::ios::app );
									ofile << sfc << "\t" << str_path << std::endl;
									ofile.close();
								}
							}
						}
					}
				}
			}
		} 
    
		catch (...)
		{
			if ( mode == "debug" )
			{
				std::ofstream efile ( "exceptions.txt", std::ios::app );
				efile << "boost::filesystem file exception: " << str_path << std::endl;
				efile.close();
			}
			else
				;
		}
		  
	}
}


// Define Callbacks here ##############################################


inline void rbt_quit( Fl_Widget* w )
{
	// Closes TCHunt when user selects x button. Required.
	exit( 1 );
}


inline void rbt_save( Fl_Widget* w, void* op)
{
	// Localize file chooser
	Fl_File_Chooser::all_files_label = lang::all.c_str();
	Fl_File_Chooser::favorites_label = lang::favorites.c_str();
	Fl_File_Chooser::add_favorites_label = lang::add_favorites.c_str();
	Fl_File_Chooser::filename_label = lang::save.c_str();
	Fl_File_Chooser::preview_label = lang::preview.c_str();
	Fl_File_Chooser::show_label = lang::show.c_str();
	Fl_File_Chooser::save_label = lang::save.c_str();
	Fl_File_Chooser::custom_filter_label = lang::custom.c_str();
	Fl_File_Chooser::filesystems_label = lang::my_computer.c_str();
	Fl_File_Chooser::new_directory_label = lang::new_folder.c_str();
	Fl_File_Chooser::manage_favorites_label = lang::manage.c_str();
	Fl_File_Chooser::new_directory_tooltip = lang::new_folder.c_str();
	
	Fl_File_Chooser chooser( ".", "*", Fl_File_Chooser::CREATE, lang::save.c_str() );
	
	// Localize file chooser
	chooser.ok_label(lang::ok.c_str());
	
	// Uncheck preview box
	chooser.preview(0);
	
	// Show file chooser
	chooser.show();

	// Wait for user
	while( chooser.shown() )
	{ 
		Fl::wait(); 
	}
		
	// User clicked cancel. 
	if ( chooser.value() == NULL )
	{ 
		return;
	}
	
	Fl_Browser* b = ( Fl_Browser* )op;
	
	// Print out report
	std::ofstream myfile ( chooser.value() );
	
	myfile << "<!DOCTYPE html PUBLIC '-//W3C//DTD HTML 4.01 Transitional//EN'>" << std::endl;
	myfile << "<html>" << std::endl;
	myfile << "<head>" << std::endl;
	myfile << "<meta http-equiv='content-type' content='text/html; charset=utf-8'>" << std::endl;
	myfile << "<title>"<< lang::version_date << "</title>" << std::endl;
	myfile << "</head>" << std::endl;
	myfile << "<body>" << std::endl;
	myfile << "<h1><img alt='TCHunt Icon' src='http://www.16s.us/pics/TCHunt.gif'> " << lang::version_date << "</h1>" << std::endl;
	myfile << "<hr>" << std::endl;
		
	// loop writing all lines (except last line) to report file
	for ( int t=1; t != b->size(); ++t ) 
	{
		b->select(t);

		if ( b->selected(t) ) 
		{
			myfile << b->text(t) << "<br>" << std::endl;
		}
	}
	
	// Write cleaned-up last line
	b->select( b->size() );	
	std::string last_line = b->text( b->size() );	
	myfile << "<b><font color='red'>" << last_line.substr( 5, last_line.size() ) << "</font></b><br>" << std::endl;	
		
	myfile << "<hr>" << std::endl;
	myfile << "<center><small><a title='16 Systems &reg;' href='http://www.16s.us/TCHunt/'>16 Systems</a>&reg;</small></center>" << std::endl;
	myfile << "</body>" << std::endl;
	myfile << "</html>" << std::endl;
	myfile.close();
}


std::string rbt_select()
{
	// Localize chooser
	Fl_File_Chooser::all_files_label = lang::all.c_str();
	Fl_File_Chooser::favorites_label = lang::favorites.c_str();
	Fl_File_Chooser::add_favorites_label = lang::add_favorites.c_str();
	Fl_File_Chooser::filename_label = lang::folder.c_str();
	Fl_File_Chooser::preview_label = lang::preview.c_str();
	Fl_File_Chooser::show_label = lang::show.c_str();
	Fl_File_Chooser::save_label = lang::save.c_str();
	Fl_File_Chooser::custom_filter_label = lang::custom.c_str();
	Fl_File_Chooser::filesystems_label = lang::my_computer.c_str();
	Fl_File_Chooser::new_directory_label = lang::new_folder.c_str();
	Fl_File_Chooser::manage_favorites_label = lang::manage.c_str();
	Fl_File_Chooser::new_directory_tooltip = lang::new_folder.c_str();
	
	Fl_File_Chooser chooser( "/", NULL, Fl_File_Chooser::DIRECTORY, ( lang::version_date + " - " + lang::select_folder ).c_str() );
	
	// Localize chooser
	chooser.ok_label( lang::ok.c_str() );
	
	// Uncheck preview box
	chooser.preview( 0 );
	
	// Show file chooser
	chooser.show();

	// Wait for user
	while( chooser.shown() )
	{ 
		Fl::wait(); 
	}
		
	// User clicked cancel or did not select a folder. 
	if ( chooser.value() == NULL )
	{ 
		return "no_pick_rbt";
	}
	
	else
	{
		return chooser.value();
	}
}


// Main #######################################################

int main()
{
	
	// German - Done!!!
	if ( build::lang == "de" )
	{
		std::locale( "" );

		lang::copyright = "\xA9 2007-2011 '16 Systems' \xAE. Alle Rechte vorbehalten.\n\n";
                lang::license = "TCHunt\n\nSoftware-Lizenz - http://www.gnu.org/licenses/gpl.txt\n"
				"Akzeptieren Sie die Lizenzbedingungen?";    
		lang::yes = "Suche, bitte warten...";
		lang::end = "@C1@bFertig";
		lang::y = "Ja";
		lang::n = "Nein";
		lang::save = "Speichern";
			
		// Added to localize report saving GUI
		lang::all = "*";
		lang::favorites = "Favoriten";
		lang::preview = "Vorschau";
		lang::ok = "OK";
		lang::cancel = "Abbrechen";
		lang::show = " ";	// Leave Blank
		lang::custom = " "; // Leave Blank
		lang::my_computer = "Computer";
		lang::new_folder = "Neuer Ordner";
		lang::close = "schließen";
		lang::manage = "Verwalten";
		lang::add_favorites = "hinzufügen";
		lang::select_folder = "Ordner";
		lang::folder = "Ordner";
		
		if ( build::mode == "debug" )
		{
			fl_alert( std::locale("").name().c_str() );
		}
			
	}
	
	// English - Done!!!
	if ( build::lang == "en" )
	{
		std::locale( "" );

		lang::copyright = "\xA9 2007-2011 '16 Systems' \xAE. All rights reserved.\n\n";
                lang::license = "TCHunt\n\nSoftware License - http://www.gnu.org/licenses/gpl.txt\n"
				"Do you accept?";    
		lang::yes = "Searching please wait...";
		lang::end = "@C1@bFinished";
		lang::y = "Yes";
		lang::n = "No";
		lang::save = "Save";
			
		// Added to localize report saving GUI
		lang::all = "*";
		lang::favorites = "Favorites";
		lang::preview = "Preview";
		lang::ok = "OK";
		lang::cancel = "Cancel";
		lang::show = " ";	// Leave Blank
		lang::custom = " "; // Leave Blank
		lang::my_computer = "My Computer";
		lang::new_folder = "Create New Folder";
		lang::close = "Close";
		lang::manage = "Manage";
		lang::add_favorites = "Add";
		lang::select_folder = "Select Folder To Search";
		lang::folder = "Folder";
		
		if ( build::mode == "debug" )
		{
			fl_alert( std::locale("").name().c_str() );
		}
			
	}

	// French - Done!!!
	if ( build::lang == "fr" )
	{
		std::locale( "" );
		
		lang::copyright = "\xA9 2007-2011 '16 Systems' \xAE. Tous droits réservés.\n\n";
		lang::license = "TCHunt\n\nLicence de logiciel - http://www.gnu.org/licenses/gpl.txt\n"
				"Acceptez-vous la Licence?";    
		lang::yes = "Recherche en cours, patientez svp...";
		lang::end = "@C1@bRecherche Terminée";
		lang::y = "Oui";
		lang::n = "Non";
		lang::save = "Enregistrer";
		
		// Added to localize report saving GUI
		lang::all = "*";
		lang::favorites = "Favoris";
		lang::preview = "Aperçu";
		lang::ok = "OK";
		lang::cancel = "Annuler";
		lang::show = " "; // Leave Blank
		lang::custom = " "; // Leave Blank
		lang::my_computer = "Poste de Travail";
		lang::new_folder = "Nouveau Dossier";
		lang::close = "Fermer";
		lang::manage = "Gérer";
		lang::add_favorites = "Ajouter";
		lang::select_folder = "Dossier";
		lang::folder = "Dossier";
		
		if ( build::mode == "debug" )
		{
			fl_alert( std::locale("").name().c_str() );
		}
	}
	
	if ( build::lang == "es" )
	{
		std::locale( "" );
		
		lang::copyright = "\xA9 2007-2011 '16 Systems' \xAE. Todos los derechos reservados.\n\n";
		lang::license = "TCHunt\n\nLicencia de software - http://www.gnu.org/licenses/gpl.txt\n"
				"¿Lo acepta?";    
		lang::yes = "Buscando, por favor espere...";
		lang::end = "@C1@bTerminado";
		lang::y = "Sí";
		lang::n = "No";
		lang::save = "Guardar";
		
		// Added to localize report saving GUI
		lang::all = "*";
		lang::favorites = "Favoritos";
		lang::preview = "Previsualizar";
		lang::ok = "Ok";
		lang::cancel = "Cancelar";
		lang::show = " "; // Leave Blank
		lang::custom = " "; // Leave Blank
		lang::my_computer = "Mi Computadora";
		lang::new_folder = "Nueva Carpeta";
		lang::close = "Cerrar";
		lang::manage = "Administrar";
		lang::add_favorites = "Agregar";
		lang::select_folder = "Carpeta";
		lang::folder = "Carpeta";
		
		if ( build::mode == "debug" )
		{
			fl_alert( std::locale("").name().c_str() );
		}
	}
  
	// Italian - Done!!!
	if ( build::lang == "it" )
	{
		std::locale( "" );
		
		lang::copyright =   "\xA9 2007-2011 '16 Systems' \xAE. Tutti i diritti riservati.\n\n";
                lang::license = "TCHunt\n\nLicenza Software - http://www.gnu.org/licenses/gpl.txt\n"
				"Accetti?";    
		lang::yes = "Ricerca in corso, attendere...";
		lang::end = "@C1@bTerminato";
		lang::y = "Sì";
		lang::n = "No";
		lang::save = "Salva";
		
		// Added to localize report saving GUI
		lang::all = "*";
		lang::favorites = "Preferiti";
		lang::preview = "Anteprima";
		lang::ok = "OK";
		lang::cancel = "Annulla";
		lang::show = " "; // Leave Blank
		lang::custom = " "; // Leave Blank
		lang::my_computer = "Risorse del Computer";
		lang::new_folder = "Nuova Cartella";
		lang::close = "Chiudi";
		lang::manage = "Gestisci";
		lang::add_favorites = "Aggiungi";
		lang::select_folder = "Cartella";
		lang::folder = "Cartella";
		
		if ( build::mode == "debug" )
		{
			fl_alert( std::locale("").name().c_str() );
		}
	}
	
	// Indonesian - Done!!!
	if ( build::lang == "in" )
	{
		std::locale( "" );
		
		lang::copyright =   "\xA9 2007-2011 '16 Systems' \xAE. Hak cipta.\n\n";
                lang::license = "TCHunt\n\nLisensi software - http://www.gnu.org/licenses/gpl.txt\n"
				"Apakah anda setuju?";    
		lang::yes = "Sedang mencari, mohon tunggu";
		lang::end = "@C1@bSelesai";
		lang::y = "Ya";
		lang::n = "Tidak";
		lang::save = "Simpan";
		
		// Added to localize report saving GUI
		lang::all = "*";
		lang::favorites = "Favorites";
		lang::preview = "Lihat";
		lang::ok = "OK";
		lang::cancel = "Batal";
		lang::show = " "; // Leave Blank
		lang::custom = " "; // Leave Blank
		lang::my_computer = "Komputer saya";
		lang::new_folder = "Folder baru";
		lang::close = "Tutup";
		lang::manage = "Kontrol";
		lang::add_favorites = "Tambah";
		lang::select_folder = "Folder";
		lang::folder = "Folder";
		
		if ( build::mode == "debug" )
		{
			fl_alert( std::locale("").name().c_str() );
		}
	}
	
	if ( build::lang == "pt" )
	{
		std::locale( "" );
		
		lang::copyright =   "\xA9 2007-2011 '16 Systems' \xAE. Todos os direitos reservados.\n\n";
                lang::license = "TCHunt\n\nLicença de Software - http://www.gnu.org/licenses/gpl.txt\n"
				" Vocês aceitam?";    
		lang::yes = "Pesquisando por favor aguarde...";
		lang::end = "@C1@bTerminado";
		lang::y = "Sim";
		lang::n = "Não";
		lang::save = "Salvar";
		
		// Added to localize report saving GUI
		lang::all = "*";
		lang::favorites = "Favoritos";
		lang::preview = "Visualizar";
		lang::ok = "OK";
		lang::cancel = "Cancelar";
		lang::show = " "; // Leave Blank
		lang::custom = " "; // Leave Blank
		lang::my_computer = "Meu Computador";
		lang::new_folder = "Nova Pasta";
		lang::close = "Fechar";
		lang::manage = "Gerir";
		lang::add_favorites = "Adicionar";
		lang::select_folder = "Pasta";
		lang::folder = "Pasta";
		
		if ( build::mode == "debug" )
		{
			fl_alert( std::locale("").name().c_str() );
		}
	}

	if ( build::lang == "jp" )
	{
		std::locale( "" );
		
		//Fl::set_font(FL_HELVETICA, "Kochi Gothic");
		lang::copyright =   "\xA9 2007-2011 '16 Systems' \xAE. \xE8\xA4\x87\xE8\xA3\xBD\xE6\xA8\xA9\xE6\x89\x80\xE6\x9C\x89\n\n";
                lang::license = "TCHunt\n\n\xE3\x82\xBD\xE3\x83\x95\xE3\x83\x88\xE3\x82\xA6\xE3\x82\xA7\xE3\x82\xA2\xE5\x85\x8D\xE8\xA8\xB1\xE8\xA8\xBC - http://www.gnu.org/licenses/gpl.txt\n"
				"\xE5\x8F\x97\xE3\x81\x91\xE5\x85\xA5\xE3\x82\x8C\xE3\x82\x8B\xE3\x81\x8B?";    
		lang::yes = "\xE6\x8D\x9C\xE5\xBE\x85\xE3\x81\xA1\xE3\x81\xAA\xE3\x81\x95\xE3\x81\x84...";
		lang::end = "@C1@b\xE7\xB5\x82\xE4\xBA\x86\xE3\x81\x99\xE3\x82\x8B";
		lang::y = "\xE3\x81\xAF\xE3\x81\x84";
		lang::n = "\xE3\x81\x84\xE3\x81\x84\xE3\x81\x88";
		lang::save = "\xE3\x82\x92\xE9\x99\xA4\xE3\x81\x91\xE3\x81\xB0";
		
		// Added to localize report saving GUI
		lang::all = "*";
		lang::favorites = "\xE6\x9C\xAC\xE5\x91\xBD";
		lang::preview = "\xE4\xBA\x88\xE5\x91\x8A\xE7\xB7\xA8";
		lang::ok = "OK"; // Leave OK
		lang::cancel = "\xE3\x82\xAD\xE3\x83\xA3\xE3\x83\xB3\xE3\x82\xBB\xE3\x83\xAB";
		lang::show = " "; // Leave Blank
		lang::custom = " "; // Leave Blank
		lang::my_computer = "\xE3\x83\x9E\xE3\x82\xA4\xE3\x82\xB3\xE3\x83\xB3\xE3\x83\x94\xE3\x83\xA5\xE3\x83\xBC\xE3\x82\xBF";
		lang::new_folder = "\xE3\x83\x8B\xE3\x83\xA5\xE3\x83\xBC\xE3\x83\x95\xE3\x82\xA9\xE3\x83\xAB\xE3\x83\x80";
		lang::close = "\xE7\xB5\x82\xE7\xB5\x90";
		lang::manage = "\xE7\xB5\x8C\xE5\x96\xB6\xE3\x81\x99\xE3\x82\x8B";
		lang::add_favorites = " ";
		lang::select_folder = "";
		lang::folder = "";
		
		if ( build::mode == "debug" )
		{
			fl_alert( std::locale("").name().c_str() );
		}
		
		// 1. Babelfish - English to Japanese.
		// 2. Paste Japanese chars here: http://people.w3.org/rishida/scripts/uniview/uniview.php
		// 3. Paste hexidecimal output here: http://www.unicode.org/cgi-bin/GetUnihanData.pl
	}
	
	// Chinese - Done!!!
	if ( build::lang == "cn" )
	{
		std::locale( "" );
		
		//Fl::set_font(FL_HELVETICA, "Kochi Gothic");
		lang::copyright =   "\xA9 2007-2011 '16 Systems' \xAE. \xE7\x89\x88\xE6\x9D\x83\xE6\x89\x80\xE6\x9C\x89\n\n";
                lang::license = "TCHunt\n\n\xE8\xBD\xAF\xE4\xBB\xB6\xE6\x8E\x88\xE6\x9D\x83 - http://www.gnu.org/licenses/gpl.txt\n"
				"\xE6\x82\xA8\xE6\x98\xAF\xE5\x90\xA6\xE6\x8E\xA5\xE5\x8F\x97?";    
		lang::yes = "\xE6\xAD\xA3\xE5\x9C\xA8\xE6\x90\x9C\xE7\xB4\xA2, \xE8\xAF\xB7\xE7\xAD\x89\xE5\xBE\x85...";
		lang::end = "@C1@b\xE5\xB7\xB2\xE5\xAE\x8C\xE6\x88\x90";
		lang::y = "\xE6\x98\xAF";
		lang::n = "\xE5\x90\xA6";
		lang::save = "\xE4\xBF\x9D\xE5\xAD\x98";
		
		// Added to localize report saving GUI
		lang::all = "*";
		lang::favorites = "\xE6\x94\xB6\xE8\x97\x8F";
		lang::preview = "\xE9\xA2\x84\xE8\xA7\x88";
		lang::ok = "\xE7\xA1\xAE\xE5\xAE\x9A";
		lang::cancel = "\xE5\x8F\x96\xE6\xB6\x88";
		lang::show = " "; // Leave Blank
		lang::custom = " "; // Leave Blank
		lang::my_computer = "\xE6\x88\x91\xE7\x9A\x84\xE7\x94\xB5\xE8\x84\x91";
		lang::new_folder = "\xE6\x96\xB0\xE6\x96\x87\xE4\xBB\xB6\xE5\xA4\xB9";
		lang::close = "\xE5\x85\xB3\xE9\x97\xAD";
		lang::manage = "\xE7\xAE\xA1\xE7\x90\x86";
		lang::add_favorites = "\xE6\xB7\xBB\xE5\x8A\xA0";
		lang::select_folder = "";
		lang::folder = "";
		
		if ( build::mode == "debug" )
		{
			fl_alert( std::locale("").name().c_str() );
		}
	}
	  
	// Gui setup		
	Fl_Window *win = new Fl_Window( 450, 245, lang::version_date.c_str() );
	Fl_Box* box = new Fl_Box( 2, 9, 448, 25, lang::copyright.c_str() );
	Fl_Button *save_but = new Fl_Button( 2,30,100,25,lang::save.c_str() ); 
	save_but->callback( rbt_save );
	Fl_Browser *op = new Fl_Browser( 2, 60, 448, 184 ); 
	
	// Localize GUI
	fl_cancel = lang::cancel.c_str();
	fl_close = lang::close.c_str();
	fl_no = lang::n.c_str();
	fl_ok = lang::ok.c_str();
	fl_yes = lang::y.c_str();
  
	// Make user click 'Yes' to software license agreement. Otherwise, exit. 
	if ( fl_choice( lang::license.c_str(), lang::n.c_str(), 0, lang::y.c_str() ) == 2 )
	{	
		// Pick Folder to Search. If they select 'Cancel', pick is NULL
		std::string pick = rbt_select();
		
		if ( pick == "no_pick_rbt" )
			return Fl::run();
		
		// Fix FLTK bug (add / to end of path on drives C:, D:, etc.)
		std::string r( pick );
		std::reverse( r.begin(), r.end() );
		
		if ( r[0] == ':' )
			pick.append("/");

		if ( build::mode == "debug" )
		{
			fl_alert( pick.c_str() );
		}
    
		// Add Date and time to report.
		boost::posix_time::ptime time1 = boost::posix_time::second_clock::local_time();
		std::string time = boost::posix_time::to_iso_extended_string(time1);		
		op->add(time.c_str());
		
		// Show main window and set x callback
		box->align( Fl_Align( FL_ALIGN_BOTTOM_LEFT|FL_ALIGN_INSIDE ) );
		win->resizable( op );
		save_but->deactivate();
		
		// Don't allow user to make Window too small
		win->size_range( 450, 245, 0, 0, 0, 0, 0);
		
		win->show();
		win->callback( rbt_quit );
		save_but->callback( rbt_save, (void*)op );
    
		// Ask user to wait... while we search.
		op->add( lang::yes.c_str() );
		op->add( "\n" );

		// Use 19456 (19KB minimum) for debugging
		unsigned int tc_min_file_size = 19456;
		
		// Use 1048576*5 (5 MB minimum) for release
		if ( build::mode == "release" )
			tc_min_file_size = 1048576*5;		
 		
		// Create report files when in debug mode.
		cf( build::mode );
 		
		// Suspect File Count is the number of files found that contain the four TCHunt attributes
		int sfc = 0;

		files( pick.c_str(), tc_min_file_size, sfc, build::mode, op );	 		
		  
		// Show user TCHunt has finished and activate save button.
		op->add("\n");
		op->add( ( lang::end + " " + pick ).c_str() );
		save_but->activate();
	}
  
	return Fl::run();  
}
