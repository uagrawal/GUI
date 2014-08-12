
/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2013 Open Ephys

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/



#include "JuliaProcessor.h"
#include "Editors/JuliaProcessorEditor.h"
#include <stdio.h>
#include <julia.h>


JuliaProcessor::JuliaProcessor()
    : GenericProcessor("Julia Processor") //, threshold(200.0), state(true)

{

    //parameters.add(Parameter("thresh", 0.0, 500.0, 200.0, 0));
    jl_init("/home/jvoigts/julia/usr/bin");
    JL_SET_STACK_BASE;

    jl_eval_string("include(\"juliaProcessor.jl\")"); // this runs the funciton definition in the file

    //jl_eval_string("reload(\"juliaProcessor.jl\")"); // this runs the funciton definition in the file

    // this should allow the user to select a file

		/*
        jl_function_t *func = jl_get_function(jl_base_module, "sqrt");
        jl_value_t* argument = jl_box_float64(2.0);
        jl_value_t* ret = jl_call1(func, argument);

        if (jl_is_float64(ret)) {
            double retDouble = jl_unbox_float64(ret);
            printf("sqrt(2.0) in C: %e\n", retDouble);
        }
        */

}

JuliaProcessor::~JuliaProcessor()
{
    jl_exit(0);
}

AudioProcessorEditor* JuliaProcessor::createEditor()
{
    editor = new JuliaProcessorEditor(this, true);

    return editor;

}

void JuliaProcessor::setParameter(int parameterIndex, float newValue)
{
    editor->updateParameterButtons(parameterIndex);

    //Parameter& p =  parameters.getReference(parameterIndex);
    //p.setValue(newValue, 0);

    //threshold = newValue;

    //std::cout << float(p[0]) << std::endl;
    editor->updateParameterButtons(parameterIndex);
}

void JuliaProcessor::setFile(String fullpath)
{

    filePath = fullpath;

  //  const char* path = filePath.getCharPointer();

}


String JuliaProcessor::getFile()
{
    return filePath;
}

void JuliaProcessor::process(AudioSampleBuffer& buffer,
                               MidiBuffer& events,
                               int& nSamples)
{

	jl_function_t *func = jl_get_function(jl_main_module, "myprocess");    


	for (int i = 2; i < buffer.getNumChannels(); i++)
    {
    	for (int j = 0; j < nSamples; j++)
    	{
    		jl_value_t* argument = jl_box_float64((float)buffer.getSample(i,j));
    		//jl_value_t* argument = jl_box_float64(2.0);
    		jl_value_t* ret = jl_call1(func, argument);
    		double j_out=0;
    		if (jl_is_float64(ret)) {
    			j_out= jl_unbox_float64(ret);
    		}

			if (jl_exception_occurred())
			    printf("%s \n", jl_typeof_str(jl_exception_occurred()));

    		buffer.setSample(i,j,j_out); //ch, sample,val
    	}
    }
}


void JuliaProcessor::saveCustomParametersToXml(XmlElement* parentElement)
{

    XmlElement* childNode = parentElement->createNewChildElement("FILENAME");
    childNode->setAttribute("path", getFile());

}

void JuliaProcessor::loadCustomParametersFromXml()
{

    if (parametersAsXml != nullptr)
    {
        // use parametersAsXml to restore state

        forEachXmlChildElement(*parametersAsXml, xmlNode)
        {
            if (xmlNode->hasTagName("FILENAME"))
            {
                String filepath = xmlNode->getStringAttribute("path");
                JuliaProcessorEditor* fre = (JuliaProcessorEditor*) getEditor();
                fre->setFile(filepath);

            }
        }
    }

}