##############################################################################
proc n_knob_empty_string {s} {
	if {![string compare $s "empty"]} {
        return ""
    } else {
        return $s
    }
}

##############################################################################
proc n_knob_hex2int {hex} {
	scan [string range $hex 1 2] %x decimal
	set r $decimal
	scan [string range $hex 3 4] %x decimal
	set g $decimal
	scan [string range $hex 5 6] %x decimal
	set b $decimal
	if {$r < 1} {
        set r 1
    }
	if {$g < 1} {
        set g 1
    }
	if {$b < 1} {
        set b 1
    }
	return [expr ((($r << 16) + ($g << 8) + $b) * -1)] 
}

##############################################################################
proc n_knob_this_integer {s default} {
	if {![string is integer -strict $s]} {
        return $default
    } else {
        return [string trim $s]
    }
}	

##############################################################################
proc n_knob_this_num {s default} {
	if {![string is double $s]} {
        return $default
    } else {
        return [string trim $s]
    }
}	

##############################################################################
proc n_knob_this_string {s} {
	set buf [string map {" " _} [string trim $s]]
	set buf [string map {\$ \\$} $buf]
	if {[string length $s] == "0"} {
        return "empty"
    } else {
        return $buf
    }
}	

##############################################################################
proc n_knob_apply {id} {
	set vid             [string trimleft $id .]
	set v_filename      [concat v_filename_$vid]
	set v_orientation   [concat v_orientation_$vid]
	set v_frames        [concat v_frames_$vid]
	set v_min           [concat v_min_$vid]
	set v_max           [concat v_max_$vid]
	set v_step          [concat v_step_$vid]
	set v_default_state [concat v_default_state_$vid]
	set v_resolution    [concat v_resolution_$vid]
	set v_snd           [concat v_snd_$vid]
	set v_rcv           [concat v_rcv_$vid]
	set v_lab           [concat v_lab_$vid]
	set v_lab_fs        [concat v_lab_fs_$vid]
	set v_ldx           [concat v_ldx_$vid]
	set v_ldy           [concat v_ldy_$vid]
	set v_lcol          [concat v_lcol_$vid]
	set v_init          [concat v_init_$vid]
	set v_num_vis       [concat v_num_vis_$vid]
	set v_numw          [concat v_numw_$vid]
	set v_num_fs        [concat v_num_fs_$vid]
	set v_ndx           [concat v_ndx_$vid]
	set v_ndy           [concat v_ndy_$vid]
	set v_ncol          [concat v_ncol_$vid]

	global $v_filename      
	global $v_orientation   
	global $v_frames        
	global $v_min           
	global $v_max           
	global $v_step          
	global $v_default_state 
	global $v_resolution    
	global $v_snd           
	global $v_rcv           
	global $v_lab           
	global $v_lab_fs            
	global $v_ldx           
	global $v_ldy           
	global $v_lcol          
	global $v_init          
	global $v_num_vis           
	global $v_numw           
	global $v_num_fs            
	global $v_ndx           
	global $v_ndy           
	global $v_ncol          

	set b_filename       [n_knob_this_string  [set $v_filename]]
	set $v_orientation   [n_knob_this_integer [set $v_orientation] 0]
	set $v_frames        [n_knob_this_integer [set $v_frames] 1]
	set $v_min           [n_knob_this_num     [set $v_min] 0]
	set $v_max           [n_knob_this_num     [set $v_max] 1]
	set $v_step          [n_knob_this_num     [set $v_step] 0]
	set $v_default_state [n_knob_this_num     [set $v_default_state] 1]
	set $v_resolution    [n_knob_this_integer [set $v_resolution] 1]
	set b_snd            [n_knob_this_string  [set $v_snd]]
	set b_rcv            [n_knob_this_string  [set $v_rcv]]
	set b_lab            [n_knob_this_string  [set $v_lab]]
	set $v_lab_fs        [n_knob_this_integer [set $v_lab_fs] 11]
	set $v_ldx           [n_knob_this_integer [set $v_ldx] 0]
	set $v_ldy           [n_knob_this_integer [set $v_ldy] 0]
	set $v_lcol          [n_knob_this_integer [set $v_lcol] 0]
	set $v_init          [n_knob_this_integer [set $v_init] 0]
	set $v_num_vis       [n_knob_this_integer [set $v_num_vis] 0]
	set $v_numw          [n_knob_this_integer [set $v_numw] 0]
	set $v_num_fs        [n_knob_this_integer [set $v_num_fs] 11]
	set $v_ndx           [n_knob_this_integer [set $v_ndx] 0]
	set $v_ndy           [n_knob_this_integer [set $v_ndy] 0]
	set $v_ncol          [n_knob_this_integer [set $v_ncol] 0]

	set cmd [concat $id dialog \
                 $b_filename \
                 [eval concat $$v_orientation] \
                 [eval concat $$v_frames] \
                 [eval concat $$v_min] \
                 [eval concat $$v_max] \
                 [eval concat $$v_step] \
                 [eval concat $$v_default_state] \
                 [eval concat $$v_resolution] \
                 $b_snd \
                 $b_rcv \
                 $b_lab \
                 [eval concat $$v_lab_fs] \
                 [eval concat $$v_ldx] \
                 [eval concat $$v_ldy] \
                 [eval concat $$v_lcol] \
                 [eval concat $$v_init] \
                 [eval concat $$v_num_vis] \
                 [eval concat $$v_numw] \
                 [eval concat $$v_num_fs] \
                 [eval concat $$v_ndx] \
                 [eval concat $$v_ndy] \
                 [eval concat $$v_ncol] \
                 \;]
    
	pdsend $cmd
}

##############################################################################
proc n_knob_cancel {id} {
	set cmd [concat $id cancel \;]
	pdsend $cmd
}

##############################################################################
proc n_knob_ok {id} {
	n_knob_apply $id
	n_knob_cancel $id
}

##############################################################################
proc n_knob_csel {id sel} {
	set vid             [string trimleft $id .]
	set v_csel          [concat v_csel_$vid]
	global $v_csel

	set $v_csel $sel
	if {[set $v_csel] == "0"} {
		$id.csel.0 configure -text {[color label]} 
		$id.csel.1 configure -text {color num}
	} else {
		$id.csel.0 configure -text {color label}
		$id.csel.1 configure -text {[color num]}
	} 
}

##############################################################################
proc n_knob_set_color {id hexcol} {
	set vid             [string trimleft $id .]
	set v_lcol          [concat v_lcol_$vid]
	set v_ncol          [concat v_ncol_$vid]
	set v_csel          [concat v_csel_$vid]
	global $v_lcol
	global $v_ncol
	global $v_csel

	scan [string range $hexcol 1 2] %x decimal
	set r $decimal
	scan [string range $hexcol 3 4] %x decimal
	set g $decimal
	scan [string range $hexcol 5 6] %x decimal
	set b $decimal
	if {$r < 1} {
        set r 1
    }
	if {$g < 1} {
        set g 1
    }
	if {$b < 1} {
        set b 1
    }
	set dec [expr (($r << 16) + ($g << 8) + $b)]

    if {[set $v_csel] == "0"} {
        set $v_lcol $dec
    } else {
        set $v_ncol $dec
    }
}

##############################################################################
proc pdtk_n_knob_dialog {id filename orientation frames min max step \
                             default_state resolution snd rcv lab lab_fs ldx ldy lcol init \
                             num_vis numw num_fs ndx ndy ncol} {
	set vid             [string trimleft $id .]
	set v_filename      [concat v_filename_$vid]
	set v_orientation   [concat v_orientation_$vid]
	set v_frames        [concat v_frames_$vid]
	set v_min           [concat v_min_$vid]
	set v_max           [concat v_max_$vid]
	set v_step          [concat v_step_$vid]
	set v_default_state [concat v_default_state_$vid]
	set v_resolution    [concat v_resolution_$vid]
	set v_snd           [concat v_snd_$vid]
	set v_rcv           [concat v_rcv_$vid]
	set v_lab           [concat v_lab_$vid]
	set v_lab_fs        [concat v_lab_fs_$vid]
	set v_ldx           [concat v_ldx_$vid]
	set v_ldy           [concat v_ldy_$vid]
	set v_lcol          [concat v_lcol_$vid]
	set v_init          [concat v_init_$vid]
	set v_num_vis       [concat v_num_vis_$vid]
	set v_numw          [concat v_numw_$vid]
	set v_num_fs        [concat v_num_fs_$vid]
	set v_ndx           [concat v_ndx_$vid]
	set v_ndy           [concat v_ndy_$vid]
	set v_ncol          [concat v_ncol_$vid]

	global $v_filename      
	global $v_orientation   
	global $v_frames        
	global $v_min           
	global $v_max           
	global $v_step          
	global $v_default_state 
	global $v_resolution    
	global $v_snd           
	global $v_rcv           
	global $v_lab           
	global $v_lab_fs            
	global $v_ldx           
	global $v_ldy           
	global $v_lcol          
	global $v_init          
	global $v_num_vis           
	global $v_numw           
	global $v_num_fs            
	global $v_ndx           
	global $v_ndy           
	global $v_ncol          

	set $v_filename      $filename      
	set $v_orientation   $orientation   
	set $v_frames        $frames        
	set $v_min           $min
	set $v_max           $max
	set $v_step          $step
	set $v_default_state $default_state
	set $v_resolution    $resolution
	set $v_snd           $snd
	set $v_rcv           $rcv
	set $v_lab           $lab
	set $v_lab_fs        $lab_fs
	set $v_ldx           $ldx
	set $v_ldy           $ldy
	set $v_lcol          $lcol
	set $v_init          $init
	set $v_num_vis       $num_vis
	set $v_numw          $numw
	set $v_num_fs        $num_fs
	set $v_ndx           $ndx
	set $v_ndy           $ndy
	set $v_ncol          $ncol

	##########################################################################
	# compare string
	set $v_snd [n_knob_empty_string [set $v_snd]]
	set $v_rcv [n_knob_empty_string [set $v_rcv]]
	set $v_lab [n_knob_empty_string [set $v_lab]]

	##########################################################################
	# setup
	toplevel $id
	wm title $id {n_knob}
	wm protocol $id WM_DELETE_WINDOW [concat n_knob_cancel $id]
	label $id.lab -text {EDITOR PROPERTIES}
	pack $id.lab -side top

	##########################################################################
	# cancel apply ok
	frame $id.bf
	pack $id.bf -side bottom -fill x -pady 2m
	button $id.bf.cancel -text {Cancel} -width 10 -command "n_knob_cancel $id"
	button $id.bf.apply  -text {Apply}  -width 10 -command "n_knob_apply $id"
	button $id.bf.ok     -text {Ok}     -width 10 -command "n_knob_ok $id"
	pack $id.bf.cancel -side left -expand 1
	pack $id.bf.apply  -side left -expand 1
	pack $id.bf.ok     -side left -expand 1

	##########################################################################
	# param
	#
	frame $id.f_filename
	pack  $id.f_filename -side top
	label $id.f_filename.l -text "      filename:"
	entry $id.f_filename.e -textvariable $v_filename -width 22
	pack  $id.f_filename.l $id.f_filename.e -side left

	#
	frame $id.f_orientation
	pack  $id.f_orientation -side top
	label $id.f_orientation.l -text "orientation:"
    checkbutton $id.f_orientation.c -variable $v_orientation
	pack  $id.f_orientation.l $id.f_orientation.c -side left
    
	#
	frame $id.f_frames
	pack  $id.f_frames -side top
	label $id.f_frames.l -text "        frames:"
	entry $id.f_frames.e -textvariable $v_frames -width 22
	pack  $id.f_frames.l $id.f_frames.e -side left
    
	# val
	frame $id.val
	pack  $id.val -side top
    
	#
	frame $id.val.f_min
	pack  $id.val.f_min -side left
	label $id.val.f_min.l -text           "    min:"
	entry $id.val.f_min.e -textvariable $v_min -width 8
	pack  $id.val.f_min.l $id.val.f_min.e -side left

	#
	frame $id.val.f_max
	pack  $id.val.f_max -side left
	label $id.val.f_max.l -text           "    max:"
	entry $id.val.f_max.e -textvariable $v_max -width 8
	pack  $id.val.f_max.l $id.val.f_max.e -side left

	# def
	frame $id.def
	pack  $id.def -side top

	#
	frame $id.def.f_step
	pack  $id.def.f_step -side left
	label $id.def.f_step.l -text          "   step:"
	entry $id.def.f_step.e -textvariable $v_step -width 8
	pack  $id.def.f_step.l $id.def.f_step.e -side left

	#
	frame $id.def.f_default_state
	pack  $id.def.f_default_state -side left
	label $id.def.f_default_state.l -text "default:"
	entry $id.def.f_default_state.e -textvariable $v_default_state -width 8
	pack  $id.def.f_default_state.l $id.def.f_default_state.e -side left

	#
	frame $id.f_resolution
	pack  $id.f_resolution -side top
	label $id.f_resolution.l -text "    resolution:"
	entry $id.f_resolution.e -textvariable $v_resolution -width 22
	pack  $id.f_resolution.l $id.f_resolution.e -side left

	#
	frame $id.f_snd
	pack  $id.f_snd -side top
	label $id.f_snd.l -text    "          send:"
	entry $id.f_snd.e -textvariable $v_snd -width 22
	pack  $id.f_snd.l $id.f_snd.e -side left

	#
	frame $id.f_rcv
	pack  $id.f_rcv -side top
	label $id.f_rcv.l -text    "       receive:"
	entry $id.f_rcv.e -textvariable $v_rcv -width 22
	pack  $id.f_rcv.l $id.f_rcv.e -side left

	#
	frame $id.f_lab
	pack  $id.f_lab -side top
	label $id.f_lab.l -text    "         label:"
	entry $id.f_lab.e -textvariable $v_lab -width 22
	pack  $id.f_lab.l $id.f_lab.e -side left

	#
	frame $id.f_lab_fs
	pack  $id.f_lab_fs -side top
	label $id.f_lab_fs.l -text "label fontsize:"
	entry $id.f_lab_fs.e -textvariable $v_lab_fs -width 22
	pack  $id.f_lab_fs.l $id.f_lab_fs.e -side left

	# fl
	frame $id.fl
	pack  $id.fl -side top

	frame $id.fl.f_ldx
	pack  $id.fl.f_ldx -side left
	label $id.fl.f_ldx.l -text            "    ldx:"
	entry $id.fl.f_ldx.e -textvariable $v_ldx -width 8
	pack  $id.fl.f_ldx.l $id.fl.f_ldx.e -side left

	frame $id.fl.f_ldy
	pack  $id.fl.f_ldy -side left
	label $id.fl.f_ldy.l -text            "    ldy:"
	entry $id.fl.f_ldy.e -textvariable $v_ldy -width 8
	pack  $id.fl.f_ldy.l $id.fl.f_ldy.e -side left

	#
	frame $id.f_num_vis
	pack  $id.f_num_vis -side top
	label $id.f_num_vis.l -text "num. vis:"
    checkbutton $id.f_num_vis.c -variable $v_num_vis
	pack  $id.f_num_vis.l $id.f_num_vis.c -side left

	#
	frame $id.f_numw
	pack  $id.f_numw -side top
	label $id.f_numw.l -text   "     num width:"
	entry $id.f_numw.e -textvariable $v_numw -width 22
	pack  $id.f_numw.l $id.f_numw.e -side left

	#
	frame $id.f_num_fs
	pack  $id.f_num_fs -side top
	label $id.f_num_fs.l -text " num. fontsize:"
	entry $id.f_num_fs.e -textvariable $v_num_fs -width 22
	pack  $id.f_num_fs.l $id.f_num_fs.e -side left

	# fn
	frame $id.fn
	pack  $id.fn -side top

	frame $id.fn.f_ndx
	pack  $id.fn.f_ndx -side left
	label $id.fn.f_ndx.l -text            "    ndx:"
	entry $id.fn.f_ndx.e -textvariable $v_ndx -width 8
	pack  $id.fn.f_ndx.l $id.fn.f_ndx.e -side left

	frame $id.fn.f_ndy
	pack  $id.fn.f_ndy -side left
	label $id.fn.f_ndy.l -text            "    ndy:"
	entry $id.fn.f_ndy.e -textvariable $v_ndy -width 8
	pack  $id.fn.f_ndy.l $id.fn.f_ndy.e -side left

	#
	frame $id.f_init
	pack  $id.f_init -side top
	label $id.f_init.l -text "init:"
    checkbutton $id.f_init.c -variable $v_init
	pack  $id.f_init.l $id.f_init.c -side left

	##########################################################################
	# colors
	set v_csel [concat v_csel_$vid]
	global $v_csel
    
	frame $id.csel
	button $id.csel.0 -text {color label} -width 10 -command "n_knob_csel $id 0"
	button $id.csel.1 -text {color num} -width 10 -command "n_knob_csel $id 1"
	pack $id.csel.0 $id.csel.1 -fill y -side left -expand yes
	pack $id.csel -fill x -pady 2m
	n_knob_csel $id 0

	##########################################################################
	frame $id.colors
    foreach r {r1 r2 r3} hexcols {
		{ "#FFFFFF" "#DFDFDF" "#BBBBBB" "#FFC7C6" "#FFE3C6" \
              "#FEFFC6" "#C6FFC7" "#C6FEFF" "#C7C6FF" "#E3C6FF" }
		{ "#9F9F9F" "#7C7C7C" "#606060" "#FF0400" "#FF8300" \
              "#FAFF00" "#00FF04" "#00FAFF" "#0400FF" "#9C00FF" }
		{ "#404040" "#202020" "#000000" "#551312" "#553512" \
              "#535512" "#0F4710" "#0E4345" "#131255" "#2F004D" } } \
		{
            frame $id.colors.$r
            pack $id.colors.$r -side top
            foreach i { 0 1 2 3 4 5 6 7 8 9} hexcol $hexcols {
                button $id.colors.$r.c$i \
                    -background $hexcol \
                    -activebackground $hexcol \
                    -relief solid \
                    -padx 1 -pady 0 -width 3 \
                    -command "n_knob_set_color $id $hexcol"
            }
            pack \
                $id.colors.$r.c0 \
                $id.colors.$r.c1 \
                $id.colors.$r.c2 \
                $id.colors.$r.c3 \
                $id.colors.$r.c4 \
                $id.colors.$r.c5 \
                $id.colors.$r.c6 \
                $id.colors.$r.c7 \
                $id.colors.$r.c8 \
                $id.colors.$r.c9 -side left
        }
	pack $id.colors

	##########################################################################
	# bind focus
	bind $id <KeyPress-Return> [concat n_knob_ok $id]
	bind $id <Control-w> [concat n_knob_cancel $id]
	focus $id.f_filename.e
}
