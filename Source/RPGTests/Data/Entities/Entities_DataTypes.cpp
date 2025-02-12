
#include "Entities_DataTypes.h"

void FEntities_CommandData::ApplyBaseData(const FEntities_BaseCommandData& BaseData)
{
	SourceTransform = BaseData.SourceTransform;
	TargetTransform = BaseData.TargetTransform;
	CommandType = BaseData.CommandType;
	HasNavigation = BaseData.HasNavigation;
}