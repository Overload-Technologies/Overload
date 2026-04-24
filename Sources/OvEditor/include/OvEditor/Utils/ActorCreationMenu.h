/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <functional>
#include <optional>

namespace OvUI::Widgets::Menu
{
    class MenuList;
}

namespace OvCore::ECS
{
    class Actor;
}

namespace OvEditor::Utils
{
    /**
    * Class exposing tools to generate an actor creation menu
    */
    class ActorCreationMenu
    {
    public:
        using ActorParentProvider = std::function<OvCore::ECS::Actor*()>;

        /**
        * Disabled constructor
        */
        ActorCreationMenu() = delete;

        /**
        * Generates an actor creation menu under the given MenuList item.
        * Also handles custom additionnal OnClick callback
        * @param p_menuList
        * @param p_parentProvider
        * @param p_onItemClicked
        */
        static void GenerateActorCreationMenu(OvUI::Widgets::Menu::MenuList& p_menuList, ActorParentProvider p_parentProvider = {}, std::optional<std::function<void()>> p_onItemClicked = {});
    };
}
