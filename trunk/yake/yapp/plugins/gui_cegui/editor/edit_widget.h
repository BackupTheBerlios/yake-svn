#ifndef _EDIT_WIDGET_H
#define _EDIT_WIDGET_H

#include "CEGUI.h"

namespace yake 
{ 
namespace gui 
{
class edit_widget;
class widget_editor;
}
}

namespace plugin
{

// todo: highlight/mouse over does not work correctly, but it does for the other edit_widget cegui demo?!
class edit_widget : public CEGUI::Static
{
public:
	static struct defaults
	{
		static float placement_step_size;
		static float frame_line_width;
		static float frame_corner_size;
		static std::string imageset_name;
		static std::string mouse_cursor_move;
		static std::string mouse_cursor_resize_nw_se;
		static std::string mouse_cursor_resize_no_so;
		static std::string mouse_cursor_resize_ne_sw;
		static CEGUI::colour line_colour;
		static CEGUI::colour corner_normal_colour;
		static CEGUI::colour corner_highlight_colour;
	};

public:
	enum line_names
	{
		LINE_TOP = 0,
		LINE_RIGHT,
		LINE_BOTTOM,
		LINE_LEFT,
		LINES_COUNT
	};

	class line : public CEGUI::RenderableImage
	{
	public:
		line()
		{
			using namespace CEGUI;
			static Imageset * editor_corners = ImagesetManager::getSingleton().createImageset((utf8*)"../datafiles/imagesets/editor_point.imageset");
			setHorzFormatting(RenderableImage::HorzStretched);
			setVertFormatting(RenderableImage::VertStretched);
			setImage(&editor_corners->getImage((utf8*)"point"));
			setSize(Size(defaults::frame_line_width, defaults::frame_line_width));
			setColours(defaults::line_colour, defaults::line_colour, 
				defaults::line_colour, defaults::line_colour);	
		}

		virtual void draw(float z, const CEGUI::Rect & clip_rect)
		{ CEGUI::RenderableImage::draw(CEGUI::Vector3(0.0f, 0.0f, z), clip_rect);	}
	};
	
	enum corner_names
	{
		CORNER_TOP_LEFT = 0,
		CORNER_TOP_MIDDLE,
		CORNER_TOP_RIGHT,
		CORNER_BOTTOM_RIGHT,
		CORNER_BOTTOM_MIDDLE,
		CORNER_BOTTOM_LEFT,
		CORNERS_COUNT
	};

	class corner : public CEGUI::RenderableImage
	{
	public:
		corner() : m_highlight(false), m_sizing(false)
		{
			using namespace CEGUI;
			static Imageset * editor_corners = ImagesetManager::getSingleton().createImageset((utf8*)"../datafiles/imagesets/editor_corner.imageset");
			setHorzFormatting(RenderableImage::HorzStretched);
			setVertFormatting(RenderableImage::VertStretched);
			setImage(&editor_corners->getImage((utf8*)"corner"));
			setSize(Size(defaults::frame_corner_size, defaults::frame_corner_size));
			set_highlight(false);
		}

		bool get_highlight() const
		{ return m_highlight; }

		void set_highlight(bool highlight)
		{ 
			using namespace CEGUI;
			if(highlight)
			{ 
				setColours(defaults::corner_highlight_colour, defaults::corner_highlight_colour, 
					defaults::corner_highlight_colour, defaults::corner_highlight_colour);	
			}
			else
			{ 
				setColours(defaults::corner_normal_colour, defaults::corner_normal_colour, 
					defaults::corner_normal_colour, defaults::corner_normal_colour);	
			}
			m_highlight = highlight; 
		}

		void set_sizing(bool sizing)
		{ m_sizing = sizing; }

		bool get_sizing() const
		{ return m_sizing; }

		virtual void draw(float z, const CEGUI::Rect & clip_rect)
		{ CEGUI::RenderableImage::draw(CEGUI::Vector3(0.0f, 0.0f, z), clip_rect);	}

	private:
		bool m_highlight;
		bool m_sizing;
	};

public:
	edit_widget(const yake::gui::edit_widget & wrapper, const yake::gui::widget_editor & editor, CEGUI::Window & child);

	virtual void onActivated(CEGUI::ActivationEventArgs & e);

	void update_cursor(CEGUI::MouseEventArgs & e)
	{
		/* update corner highlighting */
		bool is_a_corner_highlighted = false;
		for(unsigned int i = 0; i < CORNERS_COUNT; i++)
		{
			if(m_corners[i].getRect().isPointInRect(e.position))
			{
				is_a_corner_highlighted = true;
				if(!m_corners[i].get_highlight())
				{
					if(i == CORNER_TOP_LEFT || i == CORNER_BOTTOM_RIGHT) 
					{	
						CEGUI::MouseCursor::getSingleton().setImage(
							&CEGUI::ImagesetManager::getSingleton()
								.getImageset(defaults::imageset_name)
									->getImage(defaults::mouse_cursor_resize_nw_se));
					}
					else if(i == CORNER_TOP_MIDDLE || i == CORNER_BOTTOM_MIDDLE) 
					{	
						CEGUI::MouseCursor::getSingleton().setImage(
							&CEGUI::ImagesetManager::getSingleton()
								.getImageset(defaults::imageset_name)
									->getImage(defaults::mouse_cursor_resize_no_so));
					}
					else 
					{	
						CEGUI::MouseCursor::getSingleton().setImage(
							&CEGUI::ImagesetManager::getSingleton()
								.getImageset(defaults::imageset_name)
									->getImage(defaults::mouse_cursor_resize_ne_sw));
					}
					// change highlighting and redraw the widget
					m_corners[i].set_highlight(true);
					requestRedraw();
				}
				break;
			}
			else if(m_corners[i].get_highlight())
			{
				// change highlighting and redraw the widget
				m_corners[i].set_highlight(false);
				requestRedraw();
			}
		}
		/* update move cursor */
		if(!is_a_corner_highlighted)
				CEGUI::MouseCursor::getSingleton().setImage(
					&CEGUI::ImagesetManager::getSingleton().getImageset(defaults::imageset_name)
						->getImage(defaults::mouse_cursor_move));
	}

	virtual void onMouseEnters(CEGUI::MouseEventArgs & e)
	{	Static::onMouseEnters(e); update_cursor(e); }

	virtual void onMouseLeaves(CEGUI::MouseEventArgs & e)
	{
		Static::onMouseLeaves(e);

		// disable dragging, sizing and highlighting
		m_dragging = false;
		for(unsigned int i = 0; i < CORNERS_COUNT; i++)
		{ 
			m_corners[i].set_sizing(false);
			m_corners[i].set_highlight(false);
		}
		// redraw this widget
		requestRedraw();
	}

	virtual void onMouseButtonDown(CEGUI::MouseEventArgs & e)
	{
		Static::onMouseButtonDown(e);

		// if the left button was pressed activate the input for the control
		if(e.button != CEGUI::LeftButton)	{ return; }
		else if(!isActive()) { activate(); }

		// if the mouse cursors hits a corner, we should do some resizing, else the user wants to move the control
		m_dragging = true; 
		for(unsigned int i = 0; i < CORNERS_COUNT; i++)
		{
			if(m_corners[i].getRect().isPointInRect(e.position))
			{	m_corners[i].set_sizing(true); m_dragging = false; break; }
		}

		// store current mouse position
		CEGUI::Window * parent = getParent();
		CEGUI::Point local_mouse_pos(parent->screenToWindow(e.position));
		local_mouse_pos = parent->relativeToAbsolute(local_mouse_pos);
		m_previos_mouse_pos.d_x = local_mouse_pos.d_x;
		m_previos_mouse_pos.d_y = local_mouse_pos.d_y;
	}

	virtual void onMouseMove(CEGUI::MouseEventArgs & e)
	{
		Static::onMouseMove(e);

		// is this widget activated?
		if(!isActive()) return;

		// update cursor icon and corner highlighting
		update_cursor(e);

		/* dragging and sizing */
		CEGUI::Window * parent = getParent();
		CEGUI::Point local_mouse_pos(parent->screenToWindow(e.position));
		local_mouse_pos = parent->relativeToAbsolute(local_mouse_pos);

		// calculate movement deltas
		float	deltaX = local_mouse_pos.d_x - m_previos_mouse_pos.d_x;
		float	deltaY = local_mouse_pos.d_y - m_previos_mouse_pos.d_y;

		// update previos mouse point
		m_previos_mouse_pos.d_x = local_mouse_pos.d_x;
		m_previos_mouse_pos.d_y = local_mouse_pos.d_y;

		// move the widget
		if(m_dragging)
		{
			// update widget
			setXPosition(CEGUI::Absolute, getAbsoluteXPosition() + deltaX);
			setYPosition(CEGUI::Absolute, getAbsoluteYPosition() + deltaY);

			// update child
			m_child.setXPosition(CEGUI::Absolute, m_child.getAbsoluteXPosition() + deltaX);
			m_child.setYPosition(CEGUI::Absolute, m_child.getAbsoluteYPosition() + deltaY);

			// done
			return;
		}			
			
		// size the widget
		for(unsigned int i = 0; i < CORNERS_COUNT; i++)
		{
			if(m_corners[i].get_sizing())
			{
				if(i == CORNER_TOP_LEFT)
				{
					// x
					setXPosition(CEGUI::Absolute, getAbsoluteXPosition() + deltaX);
					setWidth(CEGUI::Absolute, getAbsoluteWidth() - deltaX);
					m_child.setXPosition(CEGUI::Absolute, m_child.getAbsoluteXPosition() + deltaX);
					m_child.setWidth(CEGUI::Absolute, m_child.getAbsoluteWidth() - deltaX);
					// y
					setYPosition(CEGUI::Absolute, getAbsoluteYPosition() + deltaY);
					setHeight(CEGUI::Absolute, getAbsoluteHeight() - deltaY);
					m_child.setYPosition(CEGUI::Absolute, m_child.getAbsoluteYPosition() + deltaY);
					m_child.setHeight(CEGUI::Absolute, m_child.getAbsoluteHeight() - deltaY);
				}
				else if(i == CORNER_TOP_MIDDLE)
				{
					// y
					setYPosition(CEGUI::Absolute, getAbsoluteYPosition() + deltaY);
					setHeight(CEGUI::Absolute, getAbsoluteHeight() - deltaY);
					m_child.setYPosition(CEGUI::Absolute, m_child.getAbsoluteYPosition() + deltaY);
					m_child.setHeight(CEGUI::Absolute, m_child.getAbsoluteHeight() - deltaY);
				}
				else if(i == CORNER_TOP_RIGHT)
				{
					// w
					setWidth(CEGUI::Absolute, getAbsoluteWidth() + deltaX);
					m_child.setWidth(CEGUI::Absolute, m_child.getAbsoluteWidth() + deltaX);
					// y
					setYPosition(CEGUI::Absolute, getAbsoluteYPosition() + deltaY);
					setHeight(CEGUI::Absolute, getAbsoluteHeight() - deltaY);
					m_child.setYPosition(CEGUI::Absolute, m_child.getAbsoluteYPosition() + deltaY);
					m_child.setHeight(CEGUI::Absolute, m_child.getAbsoluteHeight() - deltaY);
				}
				else if(i == CORNER_BOTTOM_RIGHT)
				{
					// w
					setWidth(CEGUI::Absolute, getAbsoluteWidth() + deltaX);
					m_child.setWidth(CEGUI::Absolute, m_child.getAbsoluteWidth() + deltaX);
					// h
					setHeight(CEGUI::Absolute, getAbsoluteHeight() + deltaY);
					m_child.setHeight(CEGUI::Absolute, m_child.getAbsoluteHeight() + deltaY);
				}
				else if(i == CORNER_BOTTOM_MIDDLE)
				{
					// h
					setHeight(CEGUI::Absolute, getAbsoluteHeight() + deltaY);
					m_child.setHeight(CEGUI::Absolute, m_child.getAbsoluteHeight() + deltaY);
				}
				else if(i == CORNER_BOTTOM_LEFT)
				{
					// w
					setXPosition(CEGUI::Absolute, getAbsoluteXPosition() + deltaX);
					setWidth(CEGUI::Absolute, getAbsoluteWidth() - deltaX);
					m_child.setXPosition(CEGUI::Absolute, m_child.getAbsoluteXPosition() + deltaX);
					m_child.setWidth(CEGUI::Absolute, m_child.getAbsoluteWidth() - deltaX);
					// h
					setHeight(CEGUI::Absolute, getAbsoluteHeight() + deltaY);
					m_child.setHeight(CEGUI::Absolute, m_child.getAbsoluteHeight() + deltaY);
				}
			}
		}
	}

	virtual void onMouseButtonUp(CEGUI::MouseEventArgs & e)
	{
		Static::onMouseButtonUp(e);
		if(e.button != CEGUI::LeftButton) return;
		m_dragging = false;
		for(unsigned int i = 0; i < CORNERS_COUNT; i++)
		{ m_corners[i].set_sizing(false); }
	}

	virtual void onKeyDown(CEGUI::KeyEventArgs & e)
	{
		Static::onKeyDown(e);
		if(e.scancode == CEGUI::Key::ArrowDown)
		{
			setYPosition(CEGUI::Absolute, getAbsoluteYPosition() + defaults::placement_step_size);
			m_child.setYPosition(CEGUI::Absolute, m_child.getAbsoluteYPosition() + defaults::placement_step_size);
		}
		else if(e.scancode == CEGUI::Key::ArrowUp)
		{
			setYPosition(CEGUI::Absolute, getAbsoluteYPosition() - defaults::placement_step_size);
			m_child.setYPosition(CEGUI::Absolute, m_child.getAbsoluteYPosition() - defaults::placement_step_size);
		}
		else if(e.scancode == CEGUI::Key::ArrowRight)
		{
			setXPosition(CEGUI::Absolute, getAbsoluteXPosition() + defaults::placement_step_size);
			m_child.setXPosition(CEGUI::Absolute, m_child.getAbsoluteXPosition() + defaults::placement_step_size);
		}
		else if(e.scancode == CEGUI::Key::ArrowLeft)
		{
			setXPosition(CEGUI::Absolute, getAbsoluteXPosition() - defaults::placement_step_size);
			m_child.setXPosition(CEGUI::Absolute, m_child.getAbsoluteXPosition() - defaults::placement_step_size);
		}
	}

	virtual void drawSelf(float z)
	{
		// get whatever base class needs to render
		Static::drawSelf(z);

		// don't render if this is not active
		if(!isActive()) return;

		// get unclipped area
		CEGUI::Rect area(getUnclippedInnerRect());

		// draw lines
		for(unsigned int i = 0; i < LINES_COUNT; i++)
		{ m_lines[i].draw(z, area); }

		// draw corners
		for(unsigned int i = 0; i < CORNERS_COUNT; i++)
		{ m_corners[i].draw(z, area); }
	}

	virtual void onSized(CEGUI::WindowEventArgs & e)
	{	Static::onSized(e); resize_frame();	}

	virtual void onMoved(CEGUI::WindowEventArgs & e)
	{	Static::onSized(e); resize_frame();	}

	void resize_frame()
	{
		// get properties
		using namespace CEGUI;
		Rect area(getUnclippedInnerRect());
		float w = area.getWidth();
		float h = area.getHeight();

		// update lines
		m_lines[LINE_TOP].setPosition(Point(area.d_left+(defaults::frame_corner_size/2), area.d_top+(defaults::frame_corner_size/2)));
		m_lines[LINE_RIGHT].setPosition(Point(area.d_right-(defaults::frame_corner_size/2), area.d_top-(defaults::frame_corner_size/2)));
		m_lines[LINE_BOTTOM].setPosition(Point(area.d_left+(defaults::frame_corner_size/2), area.d_bottom-(defaults::frame_corner_size/2)));
		m_lines[LINE_LEFT].setPosition(Point(area.d_left+(defaults::frame_corner_size/2), area.d_top+(defaults::frame_corner_size/2)));

		m_lines[LINE_TOP].setSize(Size(w, defaults::frame_line_width));
		m_lines[LINE_RIGHT].setSize(Size(defaults::frame_line_width, h+defaults::frame_line_width));
		m_lines[LINE_BOTTOM].setSize(Size(w, defaults::frame_line_width));
		m_lines[LINE_LEFT].setSize(Size(defaults::frame_line_width, h));

		// update corners
		m_corners[CORNER_TOP_LEFT].setPosition(Point(area.d_left, area.d_top));
		m_corners[CORNER_TOP_MIDDLE].setPosition(Point(area.d_left+(w/2)-(defaults::frame_corner_size/2), area.d_top));
		m_corners[CORNER_TOP_RIGHT].setPosition(Point(area.d_left+w-defaults::frame_corner_size, area.d_top));
		m_corners[CORNER_BOTTOM_RIGHT].setPosition(Point(area.d_left+w-defaults::frame_corner_size, area.d_top+h-defaults::frame_corner_size));
		m_corners[CORNER_BOTTOM_MIDDLE].setPosition(Point(area.d_left+(w/2)-(defaults::frame_corner_size/2), area.d_top+h-defaults::frame_corner_size));
		m_corners[CORNER_BOTTOM_LEFT].setPosition(Point(area.d_left, area.d_top+h-defaults::frame_corner_size));
	}

private:
	const yake::gui::edit_widget & m_wrapper;
	const yake::gui::widget_editor & m_editor;
	CEGUI::Window & m_child;
	CEGUI::Point m_previos_mouse_pos;
	bool m_dragging;
	line m_lines[LINES_COUNT];
	corner m_corners[CORNERS_COUNT];
};

} // namespace plugin

#endif // _EDIT_WIDGET_H