/**************************************************************************/
/*  flat_button.cpp                                                       */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "flat_button.h"

#include "scene/main/viewport.h"
#include "scene/scene_string_names.h"

void FlatButton::_gui_input(Ref<InputEvent> p_event) {
	ERR_FAIL_COND(p_event.is_null());

	if (is_disabled()) { // no interaction with disabled button
		return;
	}

	if (p_event->is_echo()) {
		return;
	}

	for (int i = 0; i < inputs.size(); i++) {
		if (p_event->is_action(inputs[i])) {
			on_action_event(p_event, inputs[i]);
			return;
		}
	}

	Ref<InputEventMouseMotion> mouse_motion = p_event;
	if (mouse_motion.is_valid()) {
		if (status.press_attempt) {
			bool last_press_inside = status.pressing_inside;
			status.pressing_inside = has_point(mouse_motion->get_position());
			if (last_press_inside != status.pressing_inside) {
				update();
			}
		}
	}
}

void FlatButton::_notification(int p_what) {
	if (p_what == NOTIFICATION_MOUSE_ENTER) {
		status.hovering = true;
		update();
	}

	if (p_what == NOTIFICATION_MOUSE_EXIT) {
		status.hovering = false;
		update();
	}
	if (p_what == NOTIFICATION_DRAG_BEGIN || p_what == NOTIFICATION_SCROLL_BEGIN) {
		if (status.press_attempt) {
			status.press_attempt = false;
			update();
		}
	}

	if (p_what == NOTIFICATION_FOCUS_ENTER) {
		update();
	}

	if (p_what == NOTIFICATION_FOCUS_EXIT) {
		if (status.press_attempt) {
			status.press_attempt = false;
			update();
		} else if (status.hovering) {
			update();
		}
	}

	if (p_what == NOTIFICATION_EXIT_TREE || (p_what == NOTIFICATION_VISIBILITY_CHANGED && !is_visible_in_tree())) {
		status.pressed = false;
		status.hovering = false;
		status.press_attempt = false;
		status.pressing_inside = false;
	}
}

void FlatButton::_pressed(const String &event_name) {
	if (get_script_instance()) {
		get_script_instance()->call(SceneStringNames::get_singleton()->_pressed);
	}
	pressed();
	emit_signal("pressed", event_name);
}

void FlatButton::on_action_event(Ref<InputEvent> p_event, const String &event_name) {
	if (p_event->is_pressed()) {
		status.press_attempt = true;
		status.pressing_inside = true;
		emit_signal("down", event_name);
	}

	if (status.press_attempt && status.pressing_inside) {
		if ((p_event->is_pressed() && press_mode == PRESS_MODE_PRESS)
			|| (!p_event->is_pressed() && press_mode == PRESS_MODE_RELEASE)) {
			_pressed(event_name);
		}
	}

	if (!p_event->is_pressed()) {
		Ref<InputEventMouseButton> mouse_button = p_event;
		if (mouse_button.is_valid()) {
			if (!has_point(mouse_button->get_position())) {
				status.hovering = false;
			}
		}
		status.press_attempt = false;
		status.pressing_inside = false;
		emit_signal("up", event_name);
	}

	update();
}

void FlatButton::pressed() {
}

void FlatButton::set_disabled(bool p_disabled) {
	if (status.disabled == p_disabled) {
		return;
	}

	status.disabled = p_disabled;
	if (p_disabled) {
		status.pressed = false;
		status.press_attempt = false;
		status.pressing_inside = false;
	}
	update();
	_change_notify("disabled");
}

bool FlatButton::is_disabled() const {
	return status.disabled;
}

bool FlatButton::is_hovered() const {
	return status.hovering;
}

void FlatButton::set_press_mode(PressMode p_press_mode) {
	press_mode = p_press_mode;
}

FlatButton::PressMode FlatButton::get_press_mode() const {
	return press_mode;
}

void FlatButton::set_inputs(const Vector<String> &p_inputs) {
	inputs = p_inputs;
}

Vector<String> FlatButton::get_inputs() const {
	return inputs;
}

void FlatButton::set_shortcuts(const Vector<String> &p_shortcuts) {
	shortcuts = p_shortcuts;
	set_process_unhandled_input(shortcuts.size() > 0);
}

Vector<String> FlatButton::get_shortcuts() const {
	return shortcuts;
}

void FlatButton::_unhandled_input(Ref<InputEvent> p_event) {
	ERR_FAIL_COND(p_event.is_null());

	if (is_disabled()) {
		return;
	}

	if (!is_visible_in_tree()) {
		return;
	}

	if (get_viewport()->get_modal_stack_top() && !get_viewport()->get_modal_stack_top()->is_a_parent_of(this)) {
		return; //ignore because of modal window
	}

	if (p_event->is_echo()) {
		return;
	}

	Ref<InputEventMouseMotion> mouse_motion = p_event;
	if (mouse_motion.is_valid()) {
		return;
	}

	for (int i = 0; i < shortcuts.size(); i++) {
		if (p_event->is_action(shortcuts[i])) {
			on_action_event(p_event, shortcuts[i]);
			return;
		}
	}
}

void FlatButton::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_gui_input"), &FlatButton::_gui_input);
	ClassDB::bind_method(D_METHOD("_unhandled_input"), &FlatButton::_unhandled_input);

	ClassDB::bind_method(D_METHOD("is_hovered"), &FlatButton::is_hovered);

	ClassDB::bind_method(D_METHOD("set_disabled", "disabled"), &FlatButton::set_disabled);
	ClassDB::bind_method(D_METHOD("is_disabled"), &FlatButton::is_disabled);

	ClassDB::bind_method(D_METHOD("set_press_mode", "press_mode"), &FlatButton::set_press_mode);
	ClassDB::bind_method(D_METHOD("get_press_mode"), &FlatButton::get_press_mode);

	ClassDB::bind_method(D_METHOD("set_inputs", "inputs"), &FlatButton::set_inputs);
	ClassDB::bind_method(D_METHOD("get_inputs"), &FlatButton::get_inputs);

	ClassDB::bind_method(D_METHOD("set_shortcuts", "shortcuts"), &FlatButton::set_shortcuts);
	ClassDB::bind_method(D_METHOD("get_shortcuts"), &FlatButton::get_shortcuts);

	BIND_VMETHOD(MethodInfo("_pressed"));

	ADD_SIGNAL(MethodInfo("pressed", PropertyInfo(Variant::STRING, "event_name")));
	ADD_SIGNAL(MethodInfo("up", PropertyInfo(Variant::STRING, "event_name")));
	ADD_SIGNAL(MethodInfo("down", PropertyInfo(Variant::STRING, "event_name")));

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "disabled"), "set_disabled", "is_disabled");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "press_mode", PROPERTY_HINT_ENUM, "Press,Release"), "set_press_mode", "get_press_mode");
	ADD_PROPERTY(PropertyInfo(Variant::POOL_STRING_ARRAY, "inputs", PROPERTY_HINT_TYPE_STRING, ""), "set_inputs", "get_inputs");
	ADD_PROPERTY(PropertyInfo(Variant::POOL_STRING_ARRAY, "shortcuts", PROPERTY_HINT_TYPE_STRING, ""), "set_shortcuts", "get_shortcuts");

	BIND_ENUM_CONSTANT(PRESS_MODE_PRESS);
	BIND_ENUM_CONSTANT(PRESS_MODE_RELEASE);
}

FlatButton::FlatButton() {
	status.pressed = false;
	status.press_attempt = false;
	status.hovering = false;
	status.pressing_inside = false;
	status.disabled = false;
	set_focus_mode(FOCUS_ALL);
	press_mode = PRESS_MODE_RELEASE;
}

FlatButton::~FlatButton() {
}
