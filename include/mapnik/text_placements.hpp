/*****************************************************************************
 *
 * This file is part of Mapnik (c++ mapping toolkit)
 *
 * Copyright (C) 2011 Hermann Kraus
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/
#ifndef TEXT_PLACEMENTS_HPP
#define TEXT_PLACEMENTS_HPP

//mapnik
#include <mapnik/enumeration.hpp>
#include <mapnik/filter_factory.hpp>
#include <mapnik/color.hpp>
#include <mapnik/font_set.hpp>
#include <mapnik/text_path.hpp>
#include <mapnik/box2d.hpp>

//stl
#include <vector>
#include <string>
#include <queue>

//boost
#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

namespace mapnik {

class text_placements;
class string_info;
typedef text_path placement_element;

typedef boost::tuple<double,double> position;

enum label_placement_enum {
    POINT_PLACEMENT,
    LINE_PLACEMENT,
    VERTEX_PLACEMENT,
    INTERIOR_PLACEMENT,
    label_placement_enum_MAX
};

DEFINE_ENUM( label_placement_e, label_placement_enum );

enum vertical_alignment
{
    V_TOP = 0,
    V_MIDDLE,
    V_BOTTOM,
    V_AUTO,
    vertical_alignment_MAX
};

DEFINE_ENUM( vertical_alignment_e, vertical_alignment );

enum horizontal_alignment
{
    H_LEFT = 0,
    H_MIDDLE,
    H_RIGHT,
    H_AUTO,
    horizontal_alignment_MAX
};

DEFINE_ENUM( horizontal_alignment_e, horizontal_alignment );

enum justify_alignment
{
    J_LEFT = 0,
    J_MIDDLE,
    J_RIGHT,
    justify_alignment_MAX
};

DEFINE_ENUM( justify_alignment_e, justify_alignment );

enum text_transform
{
    NONE = 0,
    UPPERCASE,
    LOWERCASE,
    CAPITALIZE,
    text_transform_MAX
};

DEFINE_ENUM( text_transform_e, text_transform );

class text_placement_info : boost::noncopyable
{
public:
    text_placement_info(text_placements const* parent);
    /** Get next placement.
      * This function is also called before the first placement is tried. */
    virtual bool next()=0;
    /** Get next placement position.
      * This function is also called before the first position is used.
      * Each class has to return at least one position!
      * If this functions returns false the placement data should be considered invalid!
      */
    virtual bool next_position_only()=0;
    virtual ~text_placement_info() {}

    /* NOTE: Values are public and non-virtual to avoid any performance problems.
             Make sure to add each new attribute in the constructor of this
             class, too or you will run into problems.
       TODO: Compare speed of current solutuion and adding all these parameters to a struct. */
    expression_ptr name;

    std::string face_name;
    font_set fontset;
    unsigned text_size;

    expression_ptr orientation;

    position anchor;
    position displacement;
    label_placement_e label_placement;
    horizontal_alignment_e halign;
    justify_alignment_e jalign;
    vertical_alignment_e valign;


    unsigned line_spacing;
    unsigned character_spacing;
    unsigned label_spacing; // distance between repeated labels on a single geometry
    unsigned label_position_tolerance; //distance the label can be moved on the line to fit, if 0 the default is used
    bool avoid_edges;
    double minimum_distance;
    double minimum_padding;
    double max_char_angle_delta;
    bool force_odd_labels; //Always try render an odd amount of labels
    bool allow_overlap;
    double text_opacity;
    unsigned text_ratio;
    bool wrap_before; // wraps text at wrap_char immediately before current word
    unsigned wrap_width;
    unsigned char wrap_char;
    text_transform_e text_transform;

    color fill;
    color halo_fill;
    double halo_radius;
    double scale_factor;
    bool has_dimensions;
    std::pair<double, double> dimensions;
    void set_scale_factor(double factor) { scale_factor = factor; }
    double get_scale_factor() { return scale_factor; }
    double get_actual_label_spacing() { return scale_factor * label_spacing; }
    double get_actual_minimum_distance() { return scale_factor * minimum_distance; }
    double get_actual_minimum_padding() { return scale_factor * minimum_padding; }

    bool collect_extents;
    string_info *info; // should only be used for finding placement. doesn't necessarily match placements.vertex() values
    //Output
    box2d<double> extents;
    std::queue< box2d<double> > envelopes;
    boost::ptr_vector<placement_element> placements;
};

typedef boost::shared_ptr<text_placement_info> text_placement_info_ptr;

class text_placements
{
public:
    text_placements();
    virtual text_placement_info_ptr get_placement_info() const =0;

    virtual ~text_placements() {}
    expression_ptr name;

    std::string face_name;
    font_set fontset;
    unsigned text_size;

    expression_ptr orientation;

    position anchor;
    position displacement;
    label_placement_e label_placement;
    horizontal_alignment_e halign;
    justify_alignment_e jalign;
    vertical_alignment_e valign;


    unsigned line_spacing;
    unsigned character_spacing;
    unsigned label_spacing;
    unsigned label_position_tolerance;
    bool avoid_edges;
    double minimum_distance;
    double minimum_padding;
    double max_char_angle_delta;
    bool force_odd_labels;
    bool allow_overlap;
    double text_opacity;
    unsigned text_ratio;
    bool wrap_before;
    unsigned wrap_width;
    unsigned char wrap_char;
    text_transform_e text_transform;

    color fill;
    color halo_fill;
    double halo_radius;

    friend class text_placement_info;
};

typedef boost::shared_ptr<text_placements> text_placements_ptr;

class text_placements_info_dummy;

class text_placements_dummy: public text_placements
{
public:
    text_placement_info_ptr get_placement_info() const;
    friend class text_placement_info_dummy;
};

class text_placement_info_dummy : public text_placement_info
{
public:
    text_placement_info_dummy(text_placements_dummy const* parent) : text_placement_info(parent),
        state(0), position_state(0), parent_(parent) {}
    bool next();
    bool next_position_only();
private:
    unsigned state;
    unsigned position_state;
    text_placements_dummy const* parent_;
};



} //namespace

#endif // TEXT_PLACEMENTS_HPP
