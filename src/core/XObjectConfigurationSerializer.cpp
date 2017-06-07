/*
    cam2web - streaming camera to web

    Copyright (C) 2017, cvsandbox, cvsandbox@gmail.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include "XObjectConfigurationSerializer.hpp"

using namespace std;

XObjectConfigurationSerializer::XObjectConfigurationSerializer( ) :
    FileName( ),
    ObjectToConfigure( )
{

}

XObjectConfigurationSerializer::XObjectConfigurationSerializer( const std::string& fileName,
                                                                const std::shared_ptr<IObjectConfigurator>& objectToConfigure ) :
    FileName( fileName ),
    ObjectToConfigure( objectToConfigure )
{
}

// Save properties of the specified object into a file
XError XObjectConfigurationSerializer::SaveConfiguration( ) const
{
    XError ret  = XError::Success;

    if ( ( FileName.empty( ) ) || ( !ObjectToConfigure ) )
    {
        ret = XError::Failed;
    }
    else
    {
        FILE* file = fopen( FileName.c_str( ), "w" );

        if ( file == nullptr )
        {
            ret = XError::IOError;
        }
        else
        {
            map<string, string> properties = ObjectToConfigure->GetAllProperties( );
            bool                first = true;

            // write a simple file, where property name and value go separate lines
            for ( auto property : properties )
            {
                if ( !first )
                {
                    fprintf( file, "\n" );
                }

                fprintf( file, "%s\n%s\n", property.first.c_str( ), property.second.c_str( ) );

                first = false;
            }

            fclose( file );
        }
    }

    return ret;
}

// Load properties of the specified object from a file
XError XObjectConfigurationSerializer::LoadConfiguration( ) const
{
    XError ret  = XError::Success;

    if ( ( FileName.empty( ) ) || ( !ObjectToConfigure ) )
    {
        ret = XError::Failed;
    }
    else
    {
        FILE* file = fopen( FileName.c_str( ), "r" );

        if ( file == nullptr )
        {
            ret = XError::IOError;
        }
        else
        {
            char   buffer[256];
            string name;
            string line;
            bool   gotName = false;

            while ( fgets( buffer, sizeof( buffer ) - 1, file ) )
            {
                line = string( buffer );

                while ( ( !line.empty( ) ) &&
                        ( ( line.back( ) == ' ' )  || ( line.back( ) == '\t' ) ||
                          ( line.back( ) == '\n' ) || ( line.back( ) == '\r' ) ) )
                {
                    line.pop_back( );
                }

                if ( !line.empty( ) )
                {
                    if ( !gotName )
                    {
                        name    = line;
                        gotName = true;
                    }
                    else
                    {
                        ObjectToConfigure->SetProperty( name, line );
                        gotName = false;
                    }
                }
            }
        }
    }

    return ret;
}
