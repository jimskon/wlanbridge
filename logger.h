
#ifndef _UBNG_LOGGER_H
#define _UBNG_LOGGER_H

#include <iostream>
#include <cstdint>
#include <fstream>

class logger : public std::ostream
{
	private:

                std::ofstream log_stream;

                struct NullBuffer : public std::streambuf
		{ int overflow( int c ) { return c; } };

		class NullStream : public std::ostream
		{
			private:
				NullBuffer _null;

			public:
				NullStream() : std::ostream( &_null )
				{ ; }

				virtual ~NullStream()
				{ ; }
		};

		NullStream _null;
		int        _level;

	public:

		logger() : _level(0)
		{ ; }

		virtual ~logger()
		{ ; }

                void path( std::string p ) {
	                 log_stream.open(p.c_str(),std::ofstream::out | std::ofstream::app);
                }
  
                void level( int level )
		{ _level = level; }

                void close()
                { log_stream.close(); }
  
                std::ostream& operator() ( int user ) {
			if( user <= _level ) {
			  if (log_stream.is_open()) {
			        return log_stream;
			  } else {
				return std::clog;
			  }
			} else {
				return _null;
			}
		}
};

#endif /*_UBNG_LOGGER_H*/

/*EoF*/
