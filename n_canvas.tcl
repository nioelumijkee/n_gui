##############################################################################
proc n_canvas_empty_string {s} {
	if {![string compare $s "empty"]} {
		return ""
	} else {
		return $s
	}
}

##############################################################################
proc n_canvas_hex2int {hex} {
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
proc n_canvas_this_integer {s default} {
	if {![string is integer -strict $s]} {
		return $default
	} else {
		return [string trim $s]
	}
}	

##############################################################################
proc n_canvas_this_string {s} {
	set buf [string map {" " _} [string trim $s]]
	set buf [string map {\$ \\$} $buf]
	if {[string length $s] == "0"} {
		return "empty"
	} else {
		return $buf
	}
}	

##############################################################################
proc n_canvas_apply {id} {
	set vid [string trimleft $id .]
	set v_width [concat v_width_$vid]
	set v_height [concat v_height_$vid]
	set v_send [concat v_send_$vid]
	set v_receive [concat v_receive_$vid]
	set v_label [concat v_label_$vid]
	set v_ldx [concat v_ldx_$vid]
	set v_ldy [concat v_ldy_$vid]
	set v_fontsize [concat v_fontsize_$vid]
	set v_bcol [concat v_bcol_$vid]
	set v_fcol [concat v_fcol_$vid]
	set v_lcol [concat v_lcol_$vid]
	set v_maxel [concat v_maxel_$vid]
	global $v_width
	global $v_height
	global $v_send
	global $v_receive
	global $v_label
	global $v_ldx
	global $v_ldy
	global $v_fontsize
	global $v_bcol
	global $v_fcol
	global $v_lcol
	global $v_maxel

	set $v_width [n_canvas_this_integer [set $v_width] 10]
	set $v_height [n_canvas_this_integer [set $v_height] 10]
	set b_send [n_canvas_this_string [set $v_send]]
	set b_receive [n_canvas_this_string [set $v_receive]]
	set b_label [n_canvas_this_string [set $v_label]]
	set $v_ldx [n_canvas_this_integer [set $v_ldx] 0]
	set $v_ldy [n_canvas_this_integer [set $v_ldy] 0]
	set $v_fontsize [n_canvas_this_integer [set $v_fontsize] 11]
	set $v_maxel [n_canvas_this_integer [set $v_maxel] 1]


	set cmd [concat $id dialog \
	[eval concat $$v_width] \
	[eval concat $$v_height] \
	$b_send \
	$b_receive \
	$b_label \
	[eval concat $$v_ldx] \
	[eval concat $$v_ldy] \
	[eval concat $$v_fontsize] \
	[n_canvas_hex2int [eval concat $$v_bcol]] \
	[n_canvas_hex2int [eval concat $$v_fcol]] \
	[n_canvas_hex2int [eval concat $$v_lcol]] \
	[eval concat $$v_maxel] \
	\;]

	# puts stderr $cmd
	pdsend $cmd
}

##############################################################################
proc n_canvas_cancel {id} {
	set cmd [concat $id cancel \;]
	pdsend $cmd
}

##############################################################################
proc n_canvas_ok {id} {
	n_canvas_apply $id
	n_canvas_cancel $id
}

##############################################################################
proc n_canvas_colordst {id sel} {
	set vid [string trimleft $id .]
	set v_color_dst [concat v_color_dst_$vid]
	set v_bcol [concat v_bcol_$vid]
	set v_fcol [concat v_fcol_$vid]
	set v_lcol [concat v_lcol_$vid]
	global $v_color_dst
	global $v_bcol
	global $v_fcol
	global $v_lcol

	set $v_color_dst $sel
	if {[set $v_color_dst] == "0"} {
		$id.colordst.0 configure -text {[Back]} 
		$id.colordst.1 configure -text {Frame} 
		$id.colordst.2 configure -text {Label}
		n_canvas_paint $id [set $v_bcol]
	} elseif {[set $v_color_dst] == "1"} {
		$id.colordst.0 configure -text {Back} 
		$id.colordst.1 configure -text {[Frame]} 
		$id.colordst.2 configure -text {Label}
		n_canvas_paint $id [set $v_fcol]
	} else {
		$id.colordst.0 configure -text {Back} 
		$id.colordst.1 configure -text {Frame} 
		$id.colordst.2 configure -text {[Label]}
		n_canvas_paint $id [set $v_lcol]
	} 
}

##############################################################################
proc n_canvas_rgb {id i} {
	set vid [string trimleft $id .]
	set v_color_r [concat v_color_r_$vid]
	set v_color_g [concat v_color_g_$vid]
	set v_color_b [concat v_color_b_$vid]
	set v_color_hex [concat v_color_hex_$vid]
	set v_color_dst [concat v_color_dst_$vid]
	set v_bcol [concat v_bcol_$vid]
	set v_fcol [concat v_fcol_$vid]
	set v_lcol [concat v_lcol_$vid]
	global $v_color_r
	global $v_color_g
	global $v_color_b
	global $v_color_hex
	global $v_color_dst
	global $v_bcol
	global $v_fcol
	global $v_lcol

	set $v_color_hex [string map {" " 0} [format #%2X%2X%2X [set $v_color_r] [set $v_color_g] [set $v_color_b]]]
	$id.view configure -bg [set $v_color_hex]

	if {[set $v_color_dst] == "0"} {
		set $v_bcol [set $v_color_hex]
	} elseif {[set $v_color_dst] == "1"} {
		set $v_fcol [set $v_color_hex]
	} else {
		set $v_lcol [set $v_color_hex]
	} 
}

##############################################################################
proc n_canvas_paint {id hexcol} {
	set vid [string trimleft $id .]
	set v_color_r [concat v_color_r_$vid]
	set v_color_g [concat v_color_g_$vid]
	set v_color_b [concat v_color_b_$vid]
	global $v_color_r
	global $v_color_g
	global $v_color_b

	scan [string range $hexcol 1 2] %x decimal
	set $v_color_r $decimal
	scan [string range $hexcol 3 4] %x decimal
	set $v_color_g $decimal
	scan [string range $hexcol 5 6] %x decimal
	set $v_color_b $decimal
	n_canvas_rgb $id 0
}

##############################################################################
proc pdtk_n_canvas_dialog {id width height send receive label ldx ldy fontsize bcol fcol lcol maxel} {
	set vid [string trimleft $id .]
	set v_width [concat v_width_$vid]
	set v_height [concat v_height_$vid]
	set v_send [concat v_send_$vid]
	set v_receive [concat v_receive_$vid]
	set v_label [concat v_label_$vid]
	set v_ldx [concat v_ldx_$vid]
	set v_ldy [concat v_ldy_$vid]
	set v_fontsize [concat v_fontsize_$vid]
	set v_bcol [concat v_bcol_$vid]
	set v_fcol [concat v_fcol_$vid]
	set v_lcol [concat v_lcol_$vid]
	set v_maxel [concat v_maxel_$vid]
	global $v_width
	global $v_height
	global $v_send
	global $v_receive
	global $v_label
	global $v_ldx
	global $v_ldy
	global $v_fontsize
	global $v_bcol
	global $v_fcol
	global $v_lcol
	global $v_maxel
	set $v_width $width
	set $v_height $height
	set $v_send $send
	set $v_receive $receive
	set $v_label $label
	set $v_ldx $ldx
	set $v_ldy $ldy
	set $v_fontsize $fontsize
	set $v_bcol $bcol
	set $v_fcol $fcol
	set $v_lcol $lcol
	set $v_maxel $maxel

	##########################################################################
	# compare string
	set $v_send [n_canvas_empty_string [set $v_send]]
	set $v_receive [n_canvas_empty_string [set $v_receive]]
	set $v_label [n_canvas_empty_string [set $v_label]]

	##########################################################################
	# setup
	toplevel $id
	wm title $id {n_canvas}
	wm protocol $id WM_DELETE_WINDOW [concat n_canvas_cancel $id]

	label $id.label -text {EDITOR PROPERTIES}
	pack $id.label -side top

	##########################################################################
	# cancel apply ok
	frame $id.buttonframe
	pack $id.buttonframe -side bottom -fill x -pady 2m
	button $id.buttonframe.cancel -text {Cancel} -width 10 \
	    -command "n_canvas_cancel $id"
	button $id.buttonframe.apply -text {Apply} -width 10 \
	    -command "n_canvas_apply $id"
	button $id.buttonframe.ok -text {Ok} -width 10 \
	    -command "n_canvas_ok $id"
	pack $id.buttonframe.cancel -side left -expand 1
	pack $id.buttonframe.apply -side left -expand 1
	pack $id.buttonframe.ok -side left -expand 1

	##########################################################################
	# param
	#
	frame $id.wh
	pack $id.wh -side top

	frame $id.wh.f_width
	pack $id.wh.f_width -side left
	label $id.wh.f_width.l -text " width:"
	entry $id.wh.f_width.e -textvariable $v_width -width 4
	pack $id.wh.f_width.l $id.wh.f_width.e -side left

	frame $id.wh.f_height
	pack $id.wh.f_height -side left
	label $id.wh.f_height.l -text "height:"
	entry $id.wh.f_height.e -textvariable $v_height -width 4
	pack $id.wh.f_height.l $id.wh.f_height.e -side left

	#
	frame $id.f_send
	pack $id.f_send -side top
	label $id.f_send.l -text "   send:"
	entry $id.f_send.e -textvariable $v_send -width 25
	pack $id.f_send.l $id.f_send.e -side left

	#
	frame $id.f_receive
	pack $id.f_receive -side top
	label $id.f_receive.l -text "receive:"
	entry $id.f_receive.e -textvariable $v_receive -width 25
	pack $id.f_receive.l $id.f_receive.e -side left

	#
	frame $id.f_label
	pack $id.f_label -side top
	label $id.f_label.l -text "  label:"
	entry $id.f_label.e -textvariable $v_label -width 25
	pack $id.f_label.l $id.f_label.e -side left

	# fc
	frame $id.fc
	pack $id.fc -side top

	frame $id.fc.f_ldx
	pack $id.fc.f_ldx -side left
	label $id.fc.f_ldx.l -text "   ldx:"
	entry $id.fc.f_ldx.e -textvariable $v_ldx -width 4
	pack $id.fc.f_ldx.l $id.fc.f_ldx.e -side left

	frame $id.fc.f_ldy
	pack $id.fc.f_ldy -side left
	label $id.fc.f_ldy.l -text "   ldy:"
	entry $id.fc.f_ldy.e -textvariable $v_ldy -width 4
	pack $id.fc.f_ldy.l $id.fc.f_ldy.e -side left

	#
	frame $id.f_fontsize
	pack $id.f_fontsize -side top
	label $id.f_fontsize.l -text "    fontsize:"
	entry $id.f_fontsize.e -textvariable $v_fontsize -width 4
	pack $id.f_fontsize.l $id.f_fontsize.e -side left

	#
	frame $id.f_maxel
	pack $id.f_maxel -side top
	label $id.f_maxel.l -text    "max elements:"
	entry $id.f_maxel.e -textvariable $v_maxel -width 4
	pack $id.f_maxel.l $id.f_maxel.e -side left

	##########################################################################
	# select color destination
	set v_color_dst [concat v_color_dst_$vid]
	global $v_color_dst

	frame $id.colordst
	button $id.colordst.0 -text {Back} -width 10 -command "n_canvas_colordst $id 0"
	button $id.colordst.1 -text {Frame} -width 10 -command "n_canvas_colordst $id 1"
	button $id.colordst.2 -text {Label} -width 10 -command "n_canvas_colordst $id 2"
	pack $id.colordst.0 $id.colordst.1 $id.colordst.2 -fill y -side left -expand yes
	pack $id.colordst -fill x -pady 2m

	##########################################################################
	# view
	set v_color_hex [concat v_color_hex_$vid]
	global $v_color_hex

	frame $id.view
	pack $id.view -fill both -expand yes
	label $id.view.label -textvariable $v_color_hex
	pack $id.view.label

	##########################################################################
	# colors
	frame $id.colors
	foreach r {r1 r2 r3} hexcols {
		{ "#FFFFFF" "#DFDFDF" "#BBBBBB" "#FFC7C6" "#FFE3C6" "#FEFFC6" "#C6FFC7" "#C6FEFF" "#C7C6FF" "#E3C6FF" }
		{ "#9F9F9F" "#7C7C7C" "#606060" "#FF0400" "#FF8300" "#FAFF00" "#00FF04" "#00FAFF" "#0400FF" "#9C00FF" }
		{ "#404040" "#202020" "#000000" "#551312" "#553512" "#535512" "#0F4710" "#0E4345" "#131255" "#2F004D" } } \
		{
		frame $id.colors.$r
		pack $id.colors.$r -side top
		foreach i { 0 1 2 3 4 5 6 7 8 9} hexcol $hexcols {
		button $id.colors.$r.c$i \
		-background $hexcol \
		-activebackground $hexcol \
		-relief solid \
		-padx 1 -pady 0 -width 3 \
		-command "n_canvas_paint $id $hexcol"
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
	set v_color_r [concat v_color_r_$vid]
	set v_color_g [concat v_color_g_$vid]
	set v_color_b [concat v_color_b_$vid]
	global $v_color_r
	global $v_color_g
	global $v_color_b

	frame $id.ctrl
	scale $id.ctrl.r -relief groove -label "r" -from 255 -to 0 -width 48 \
	-variable $v_color_r -command "n_canvas_rgb $id"
	scale $id.ctrl.g -relief groove -label "g" -from 255 -to 0 -width 48 \
	-variable $v_color_g -command "n_canvas_rgb $id"
	scale $id.ctrl.b -relief groove -label "b" -from 255 -to 0 -width 48 \
	-variable $v_color_b -command "n_canvas_rgb $id"
	pack $id.ctrl.r $id.ctrl.g $id.ctrl.b -fill y -side left -expand yes
	pack $id.ctrl.r -fill y -side left  -expand yes
	pack $id.ctrl -fill both -expand yes

	##########################################################################
	# bind focus
	bind $id.wh.f_width.e <KeyPress-Return> [concat n_canvas_ok $id]
	bind $id.wh.f_height.e <KeyPress-Return> [concat n_canvas_ok $id]
	bind $id.f_send.e <KeyPress-Return> [concat n_canvas_ok $id]
	bind $id.f_receive.e <KeyPress-Return> [concat n_canvas_ok $id]
	bind $id.f_label.e <KeyPress-Return> [concat n_canvas_ok $id]
	bind $id.fc.f_ldx.e <KeyPress-Return> [concat n_canvas_ok $id]
	bind $id.fc.f_ldy.e <KeyPress-Return> [concat n_canvas_ok $id]
	bind $id.f_fontsize.e <KeyPress-Return> [concat n_canvas_ok $id]
	bind $id.f_maxel.e <KeyPress-Return> [concat n_canvas_ok $id]
	focus $id.wh.f_width.e

	##########################################################################
	n_canvas_colordst $id 0
}
