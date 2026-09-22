#include "LayoutCore/FurnitureCatalogue.h"

namespace LayoutCore
{
	const std::vector<FurnitureGroup>& GetPlaceholderCatalogue()
	{
		static const std::vector<FurnitureGroup> Catalogue = []
		{
			std::vector<FurnitureGroup> Groups;

			// Scottish seating: sofa + armchair + coffee table.
			{
				FurnitureGroup Group;
				Group.GroupId = "TartanSeatingGroup";
				Group.GroupRole = "Seating";
				Group.StyleTags = { EStyle::Scottish };
				Group.Width = 250.0;
				Group.Depth = 180.0;
				Group.Items.push_back(FurnitureItem{ "Sofa", Vec2{ 0.0, 40.0 }, 0.0, 200.0, 90.0, 15.0 });
				Group.Items.push_back(FurnitureItem{ "CoffeeTable", Vec2{ 0.0, 110.0 }, 0.0, 100.0, 50.0, 20.0 });
				Group.Items.push_back(FurnitureItem{ "Armchair", Vec2{ 110.0, 70.0 }, -90.0, 80.0, 80.0, 15.0 });
				Groups.push_back(Group);
			}

			// Japanese seating: floor cushions + low table.
			{
				FurnitureGroup Group;
				Group.GroupId = "TatamiSeatingGroup";
				Group.GroupRole = "Seating";
				Group.StyleTags = { EStyle::Japanese };
				Group.Width = 200.0;
				Group.Depth = 200.0;
				Group.Items.push_back(FurnitureItem{ "Zabuton_A", Vec2{ -50.0, 60.0 }, 0.0, 55.0, 55.0, 10.0 });
				Group.Items.push_back(FurnitureItem{ "Zabuton_B", Vec2{ 50.0, 60.0 }, 0.0, 55.0, 55.0, 10.0 });
				Group.Items.push_back(FurnitureItem{ "Chabudai", Vec2{ 0.0, 60.0 }, 0.0, 90.0, 60.0, 15.0 });
				Groups.push_back(Group);
			}

			// Dining: table + two chairs. Style-agnostic.
			{
				FurnitureGroup Group;
				Group.GroupId = "DiningGroup";
				Group.GroupRole = "Dining";
				Group.Width = 140.0;
				Group.Depth = 140.0;
				Group.Items.push_back(FurnitureItem{ "DiningTable", Vec2{ 0.0, 70.0 }, 0.0, 120.0, 80.0, 15.0 });
				Group.Items.push_back(FurnitureItem{ "DiningChair_A", Vec2{ -40.0, 30.0 }, 180.0, 45.0, 45.0, 10.0 });
				Group.Items.push_back(FurnitureItem{ "DiningChair_B", Vec2{ 40.0, 30.0 }, 180.0, 45.0, 45.0, 10.0 });
				Groups.push_back(Group);
			}

			// Storage: single cabinet. Style-agnostic.
			{
				FurnitureGroup Group;
				Group.GroupId = "StorageGroup";
				Group.GroupRole = "Storage";
				Group.Width = 90.0;
				Group.Depth = 45.0;
				Group.Items.push_back(FurnitureItem{ "Cabinet", Vec2{ 0.0, 22.5 }, 0.0, 90.0, 45.0, 10.0 });
				Groups.push_back(Group);
			}

			return Groups;
		}();

		return Catalogue;
	}
}
