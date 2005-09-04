#!/usr/bin/python


# Dictionary with common paths
commonPaths = { 'ObjectDir':'obj' }

commonIncludes = []

# Utility lists of strings are stored here
listsSet = {}

# Dictionary containing all shared libs BuildEntities
sharedLibs = {}
programs = {}

def setCommonLibInstallDir( dir ):
	print 'Setting common lib install dir to', dir
	commonPaths['LibInstallDir'] = str(dir)


def setCommonProgramInstallDir( dir ):
	print 'Setting common program install dir to', dir
	commonPaths['ProgramInstallDir'] = str(dir)


def setCommonObjectDir( dir ):
	print 'Setting common object dir to', dir
	commonPaths['ObjectDir'] = str(dir)

def setCommonLibDir( dir ):
	print 'Setting common lib dir to', dir
	commonPaths['LibDir'] = str(dir)

class BuildEntity:
	""" Class representing build entity (program, lib, shared lib)"""
	src = ''
	libadd = []
	
	def __init__( self ):
		self.src = ''
		self.libadd = []	


def readAllTextElemsToList( src_node, tagName ):
	val_list = []
	elems = src_node.getElementsByTagName( tagName )
	for idx in range( elems.length ):
		val = elems.item( idx ).firstChild.nodeValue
		val_list.append( str(val) )
		print val
	return val_list

def readListsFromElement( src_elem ):
	""" Reads all lists defined as <List>...</List> into listsSet storage  """
	print 'Reading lists...'
	lists = src_elem.getElementsByTagName( 'List' )
	for idx in range( lists.length ):
		lst = lists.item( idx )
		lst_name = lst.attributes.item(0).nodeValue #only one name
		print 'Adding list', lst_name
		items = readAllTextElemsToList( lst, 'item' )
		listsSet[ lst_name ] = items


def readProgramInstructions( src_element ):
	""" Reads program build instructions """


def readSharedLibsInstructions( src_element ):
	""" Reads shared libs build instructions """
	
	print 'Reading shared libs build instructions...'

	so_libs = src_element.getElementsByTagName( 'BuildSharedLibrary' )
	for idx in range( so_libs.length ):
		so_lib = so_libs.item( idx )
		lib_name = ''
		lib_src = ''
		for at_idx in range( so_lib.attributes.length ):
			attr = so_lib.attributes.item( at_idx )
			if attr.name == 'name':
				lib_name = attr.nodeValue
				print 'Library:', lib_name
			elif attr.name == 'src':	
				lib_src = attr.nodeValue
				print 'Library src:', lib_src

		libadds = []
		
		libadds_spec = readAllTextElemsToList( so_lib, 'libadd' )

		for la in libadds_spec:
			if listsSet.has_key( la ):
				print 'Loading libadd from list', la, 'it is:', listsSet[ la ]
				libadds = libadds + listsSet[ la ] 
			else: 
				libadds.append( la )


		lib = BuildEntity()
		lib.src = lib_src
		lib.libadd = libadds
		#adding to dictionary
		sharedLibs[ lib_name ] = lib		  
		

def readProgramInstructions( src_element ):
	""" Reads program build instructions """
	
	print 'Reading program build instructions...'

	so_libs = src_element.getElementsByTagName( 'BuildProgram' )
	for idx in range( so_libs.length ):
		so_lib = so_libs.item( idx )
		lib_name = ''
		lib_src = ''
		for at_idx in range( so_lib.attributes.length ):
			attr = so_lib.attributes.item( at_idx )
			if attr.name == 'name':
				lib_name = attr.nodeValue
				print 'Program:', lib_name
			elif attr.name == 'src':	
				lib_src = attr.nodeValue
				print 'Program src:', lib_src

		libadds = []
		
		libadds_spec = readAllTextElemsToList( so_lib, 'libadd' )

		for la in libadds_spec:
			if listsSet.has_key( la ):
				print 'Loading libadd from list', la, 'it is:', listsSet[ la ]
				libadds = libadds + listsSet[ la ] 
			else: 
				libadds.append( la )


		lib = BuildEntity()
		lib.src = lib_src
		lib.libadd = libadds
		#adding to dictionary
		programs[ lib_name ] = lib		  
		

def readBuildInstructions( src_element ):

	print 'Reading build instructions...'
	readSharedLibsInstructions( src_element )
	readProgramInstructions( src_element )	
	



# Actual parsing occurs here
import xml.dom.minidom

dom = xml.dom.minidom.parse( 'buildspec.xml' )

print 'Reading root element', dom.documentElement.tagName

common = dom.documentElement.getElementsByTagName( 'Common' )[0] #assuming only one Common

commonAttribs = common.attributes

for at_index in range( commonAttribs.length ):
	at = commonAttribs.item( at_index )

	if at.name == 'LibInstallDir':
		setCommonLibInstallDir( at.nodeValue )
	elif at.name == 'ProgramInstallDir':
		setCommonProgramInstallDir( at.nodeValue )
	elif at.name == 'ObjectDir':
		setCommonObjectDir( at.nodeValue )
	elif at.name == 'LibDir':
		setCommonLibDir( at.nodeValue )

commonIncludes = readAllTextElemsToList( common, 'include' )
readListsFromElement( dom.documentElement )
readBuildInstructions( dom.documentElement )

