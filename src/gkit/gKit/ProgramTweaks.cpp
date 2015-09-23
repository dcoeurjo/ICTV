

#include "ProgramTweaks.h"
#include "GL/GLProgramUniforms.h"


namespace gk {

TweakManager::TweakManager( ) : programs() {}
TweakManager::~TweakManager( ) {}
    
int TweakManager::tweak( GLProgram *program )
{ 
    std::pair< std::map<GLProgram *, GLProgramUniforms *>::iterator, bool > found= 
        programs.insert( std::make_pair(program, (GLProgramUniforms *) NULL) );
    
    if(found.second) 
        found.first->second= new GLProgramUniforms(program);
    
    found.first->second->update();
    found.first->second->assign();
    
    //~ printf("%s", found.first->second->summary().c_str());
    return 0; 
}

int TweakManager::doTweaks( nv::UIContext *widgets, GLProgram *program ) 
{ 
    std::map<GLProgram *, GLProgramUniforms *>::iterator found= programs.find(program);
    if(found == programs.end())
        return -1;
    
    GLProgramUniforms *uniforms= found->second;
    //~ printf("%s--\n", uniforms->summary().c_str());
    
    // generer les widgets pour visualiser et editer les uniforms du program.
    if(widgets->beginPanel(tweaks_panel, "uniforms", &tweaks_unfold))
    {
        char tmp[1024];
        
        for(unsigned int i= 0; i < uniforms->values.size(); i++)
        {
            widgets->beginGroup(nv::GroupFlags_GrowRightFromTop);
            if(uniforms->values[i]->index == 0)
                sprintf(tmp, "%s %s", glsl::type_string(uniforms->values[i]->uniform.type), uniforms->names[i].c_str());
            else
                sprintf(tmp, "%s %s +%d", glsl::type_string(uniforms->values[i]->uniform.type), uniforms->names[i].c_str(), uniforms->values[i]->index);
            
            widgets->doLabel(nv::Rect(), tmp);
            if(uniforms->values[i]->isConst())
                widgets->doLabel(nv::Rect(), "not used");
            
            else
            {
                widgets->doCheckButton(nv::Rect(), "edit", &uniforms->values[i]->edit);
                
                if(uniforms->values[i]->edit == false)
                    widgets->doLabel(nv::Rect(), uniforms->values[i]->value.c_str());
                
                else
                {
                    if(uniforms->values[i]->edit_value.empty())
                        // initialise la valeur editable, si necessaire
                        uniforms->values[i]->edit_value= uniforms->values[i]->value;
                    
                    strcpy(tmp, uniforms->values[i]->edit_value.c_str());
                    if(widgets->doLineEdit(nv::Rect(), tmp, sizeof(tmp)))
                    {
                        printf("edit '%s': %s\n", uniforms->names[i].c_str(), tmp);
                        uniforms->values[i]->edit_value.assign(tmp);
                    }
                }
            }
            widgets->endGroup();
        }
        widgets->endPanel();
    }
    
    return 0; 
}

}       // namespace
