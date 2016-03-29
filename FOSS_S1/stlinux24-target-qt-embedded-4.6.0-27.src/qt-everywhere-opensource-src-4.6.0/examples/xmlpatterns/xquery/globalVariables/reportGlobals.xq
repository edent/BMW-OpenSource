(: 
    This XQuery loads a GCC-XML file and reports the locations of all 
    global variables in the original C++ source. To run the query,
    use the command line:

    xmlpatterns reportGlobals.xq -param fileToOpen=globals.gccxml -output globals.html

    "fileToOpen=globals.gccxml" binds the file name "globals.gccxml"
    to the variable "fileToOpen" declared and used below.
:)

declare variable $fileToOpen as xs:anyURI external;
declare variable $inDoc as document-node() := doc($fileToOpen);

(: 
   This function determines whether the typeId is a complex type,
   e.g. QString. We only check whether it's a class. To be strictly
   correct, we should check whether the class has a non-synthesized 
   constructor. We accept both mutable and const types.
:)
declare function local:isComplexType($typeID as xs:string) as xs:boolean
{
    exists($inDoc/GCC_XML/Class[@id = $typeID])
    or
    exists($inDoc/GCC_XML/Class[@id = $inDoc/GCC_XML/CvQualifiedType[@id = $typeID]/@type])
};

(:
   This function determines whether the typeId is a primitive type.
:)
declare function local:isPrimitive($typeId as xs:string) as xs:boolean
{
    exists($inDoc/GCC_XML/FundamentalType[@id = $typeId])
};

(: 
   This function constructs a line for the report. The line contains
   a variable name, the source file, and the line number.
:)
declare function local:location($block as element()) as xs:string
{
    concat($inDoc/GCC_XML/File[@id = $block/@file]/@name, " at line ", $block/@line)
};

(: 
   This function generates the report. Note that it is called once
   in the <body> element of the <html> output.

   It ignores const variables of simple types but reports all others.
:)
declare function local:report() as element()+
{
    let $complexVariables as element(Variable)* := $inDoc/GCC_XML/Variable[local:isComplexType(@type)]
    return if (exists($complexVariables))
           then (<p xmlns="http://www.w3.org/1999/xhtml/">Global variables with complex types:</p>,
                 <ol xmlns="http://www.w3.org/1999/xhtml/">
                    {
                        (: For each Variable in $complexVariables... :)
                        $complexVariables/<li><span class="variableName">{string(@name)}</span> in {local:location(.)}</li>
                    }
                 </ol>)
           else <p xmlns="http://www.w3.org/1999/xhtml/">No complex global variables found.</p>

    ,

    let $primitiveVariables as element(Variable)+ := $inDoc/GCC_XML/Variable[local:isPrimitive(@type)]
    return if (exists($primitiveVariables))
           then (<p xmlns="http://www.w3.org/1999/xhtml/">Mutable global variables with primitives types:</p>,
                 <ol xmlns="http://www.w3.org/1999/xhtml/">
                    {
                        (: For each Variable in $complexVariables... :)
                        $primitiveVariables/<li><span class="variableName">{string(@name)}</span> in {local:location(.)}</li>
                    }
                 </ol>)
           else <p xmlns="http://www.w3.org/1999/xhtml/">No mutable primitive global variables found.</p>
};

(:
    This is where the <html> report is output. First 
    there is some style stuff, then the <body> element,
    which contains the call to the \c{local:report()}
    declared above.
:)
<html xmlns="http://www.w3.org/1999/xhtml/" xml:lang="en" lang="en">
    <head>
        <title>Global variables report for {$fileToOpen}</title>
    </head>
    <style type="text/css">
        .details
        {{
            text-align: left;
            font-size: 80%;
            color: blue
        }}
        .variableName
        {{
            font-family: courier;
	    color: blue
        }}
    </style>

    <body>
        <p class="details">Start report: {current-dateTime()}</p>
        {
            local:report()
        }
        <p class="details">End report: {current-dateTime()}</p>
    </body>

</html>
