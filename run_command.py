#!/bin/python
'''
Author: QIN Shuo
Date: 2016/11/1
Description:
    A script to call connectArea to run command for a batch of files
    1. register
    2. enlarge brain to fill some holes
    3. output files to a dir
    4. delete useless files
'''
import sys
import os
import string
import thread

cmd_root = "/home/qinshuo/WorkPlace/pcl_prj/itkFillTrachea/build/connectArea "

moving = "/media/D/GE_qinshuo/template/reference_brain_res.nii " #brain tempalte
fix = "" #fixed image
warp = " " #transformed brain template
preffix = "transform" #transform file preffix : [preffix]Affine.txt
brain = "/media/D/GE_qinshuo/template/reference_brain_aal.nii" #brain segmentation
warped_brain = "" #transformed brain
filled = "" #hole fill result

def walk_through_and_delete(dir,contain_list):
    '''
    Walk through a specified dir, delete files contain keywords in cotain_list
    if file not exists, surpass and continue
    '''
    for root, dirs, files in os.walk(dir): 
        for ff in files:
            for con in contain_list:
                if ff.find(con)!= -1 : 
                    # delete file
                    try:
                        os.remove(os.path.join(root, ff))
                    except:
                        pass


# basic function, not call explicitly
def get_file_list(dir,level = 2):    
    file_ls = [];
    dir_ls = [];
    for root, dirs, files in os.walk(dir): 
        for ff in files:
            if ff.find('.nii')!= -1 :
                file_ls.append(ff)
                dir_ls.append(root)
    return file_ls,dir_ls

# basic function, not call explicitly
def get_cmd(fix,moving=moving,warp=warp,preffix = preffix,brain = brain,warp_brain = warped_brain,filled = filled):
    '''
    ANTs commands is located in the same dir by default
    ants = ANTs
    warp_ants = WarpImageMultiTransform
    '''
    cmd1 = ' {0} {1} {2} {3} {4} {5} {6} '.format(moving,
                                                    fix,
                                                    warp,
                                                    preffix,
                                                    brain,
                                                    warp_brain,
                                                    filled)
    return cmd_root+cmd1

def get_command_list(dir,out_dir=''):
    if out_dir=='':
        out_dir = dir
    cmd_list = []
    file_ls,dir_ls = get_file_list(dir)
    for ff,dir in zip(file_ls,dir_ls):
        file = os.path.join(dir,ff)
        preffix = os.path.join(out_dir,ff.split('.')[0])
        warp = os.path.join(out_dir,'warp_'+ff)
        warped_brain = os.path.join(out_dir,'warped_brain'+ff)
        filled = os.path.join(out_dir,'fill_'+ff)
        cmd = get_cmd( file,
                        warp = warp,
                        preffix = preffix, 
                        warp_brain = warped_brain,
                        filled = filled )
        
        cmd_list.append(cmd)
    return cmd_list


def runn(cmd):  # no call explicitly
    print "Begin: ",cmd
    os.system(cmd)
def run_cmd_list(cmds):
    for cmd in cmds:
        #thread.start_new_thread( runn, (cmd,) )
        runn(cmd+ "  &")    


if __name__ =='__main__':
    dir = '/media/D/GE_qinshuo/GE_qinshuo'
    out_dir = '/media/D/GE_qinshuo/output'
    #cmd_list = get_command_list(dir,out_dir= out_dir)
    #run_cmd_list(cmd_list)

    lists = ['warped','warp','Affine']
    walk_through_and_delete(out_dir,lists)



