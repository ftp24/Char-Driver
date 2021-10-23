#! /bin/python
f = open( "/dev/charenc", "w+" )
f.write( "abczf" )
print( f.read() )
f.close()
