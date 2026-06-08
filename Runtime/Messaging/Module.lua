local MessagingModule = BuildTool.DefaultTargetTemplate()

MessagingModule.Name = "Waterlily.Messaging"
MessagingModule.Kind = "shared"
MessagingModule.Group = "Engine"

MessagingModule.Deps = {
    "Waterlily.Core"
}

MessagingModule.PrivateDefines = {
    "WL_MESSAGING_EXPORTS"
}

BuildTool.RegisterTargets(MessagingModule)