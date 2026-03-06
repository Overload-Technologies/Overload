/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvUI/Widgets/AWidget.h"

namespace OvUI::Widgets
{
	/**
	* DataWidget are widgets that contains a value. It is very usefull in combination with
	* DataDispatcher plugin
	*/
	template <typename T>
	class DataWidget : public AWidget
	{
	public:
		/**
		* Create a DataWidget with the data specification
		* @param p_dataHolder
		*/
		DataWidget(T& p_dataHolder) : m_data(p_dataHolder) {};

		/**
		* Draw the widget
		*/
		virtual void Draw() override;

		/**
		* Notify that the widget data has changed to allow the data dispatcher to execute its behaviour
		*/
		void NotifyChange();

	private:
		T& m_data;
	};

	template<typename T>
	inline void DataWidget<T>::Draw()
	{
		if (enabled)
		{
			TRY_GATHER(T, m_data);
			AWidget::Draw();
			TRY_PROVIDE(T, m_data);
		}
	}

	template<typename T>
	inline void DataWidget<T>::NotifyChange()
	{
		TRY_NOTIFY_CHANGE(T);
	}
}
