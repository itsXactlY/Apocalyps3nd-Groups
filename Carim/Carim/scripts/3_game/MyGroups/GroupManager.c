/**
 * My Groups - Complete Server Module
 * Alle Klassen in einer Datei für korrekte Ladereihenfolge
 */

// ============================================================================
// Enums
// ============================================================================

enum MyGroupRank
{
	TRIAL = 0,
	MEMBER = 1,
	OFFICER = 2,
	LEADER = 3
}

enum MyGroupRPCs
{
	// Client -> Server
	GROUP_CREATE = 100,
	GROUP_DISBAND,
	GROUP_INVITE,
	GROUP_ACCEPT_INVITE,
	GROUP_LEAVE,
	GROUP_KICK,
	GROUP_PROMOTE,
	GROUP_DEMOTE,
	GROUP_TRANSFER_LEADERSHIP,
	
	// Server -> Client (Notifications)
	GROUP_INFO_UPDATE = 300,
	GROUP_MEMBER_JOINED,
	GROUP_MEMBER_LEFT,
	GROUP_MEMBER_KICKED,
	GROUP_MEMBER_PROMOTED,
	GROUP_MEMBER_DEMOTED,
	GROUP_INVITE_RECEIVED,
	GROUP_INVITE_ACCEPTED,
	GROUP_INVITE_DECLINED,
	GROUP_NOTIFICATION,
	GROUP_ERROR,
}

// ============================================================================
// MyGroupMember Class
// ============================================================================

class MyGroupMember
{
	string steamId;
	string playerName;
	MyGroupRank rank;
	int joinTimestamp;
	int lastOnline;
	
	void MyGroupMember(string pSteamId, string pName, MyGroupRank pRank = MyGroupRank.TRIAL)
	{
		steamId = pSteamId;
		playerName = pName;
		rank = pRank;
		joinTimestamp = GetGame().GetTime();
		lastOnline = joinTimestamp;
	}
	
	void SetRank(MyGroupRank newRank)
	{
		rank = newRank;
	}
	
	void UpdateLastOnline()
	{
		lastOnline = GetGame().GetTime();
	}
	
	bool HasPermission(int requiredRank)
	{
		return rank >= requiredRank;
	}
}

// ============================================================================
// MyGroup Class
// ============================================================================

class MyGroup
{
	// Group Info
	string shortname;
	string name;
	string leaderId;
	int createdTimestamp;
	
	// Members
	ref array<ref MyGroupMember> members;
	
	// Settings
	bool isOpen;
	string description;
	
	void MyGroup(string pShortname, string pName, string pLeaderId)
	{
		shortname = pShortname;
		name = pName;
		leaderId = pLeaderId;
		createdTimestamp = GetGame().GetTime();
		members = new array<ref MyGroupMember>;
		isOpen = false;
		description = "";
		
		// Leader hinzufügen
		AddMember(pLeaderId, "Leader", MyGroupRank.LEADER);
	}
	
	void AddMember(string steamId, string playerName, MyGroupRank rank = MyGroupRank.TRIAL)
	{
		if (GetMember(steamId)) return;
		
		MyGroupMember member = new MyGroupMember(steamId, playerName, rank);
		members.Insert(member);
		
		Print("[MyGroups] Player " + playerName + " joined group " + shortname);
	}
	
	void RemoveMember(string steamId)
	{
		for (int i = members.Count() - 1; i >= 0; i--)
		{
			if (members.Get(i).steamId == steamId)
			{
				members.Remove(i);
				Print("[MyGroups] Player " + steamId + " removed from group " + shortname);
				break;
			}
		}
		
		if (members.Count() == 0)
		{
			MyGroupManager.GetInstance().RemoveGroup(shortname);
		}
	}
	
	MyGroupMember GetMember(string steamId)
	{
		foreach (MyGroupMember member : members)
		{
			if (member.steamId == steamId)
				return member;
		}
		return null;
	}
	
	bool IsMember(string steamId)
	{
		return GetMember(steamId) != null;
	}
	
	int GetMemberCount()
	{
		return members.Count();
	}
	
	bool IsFull()
	{
		return GetMemberCount() >= 20;
	}
	
	bool CanKick(string steamId)
	{
		MyGroupMember member = GetMember(steamId);
		if (!member) return false;
		return member.rank >= MyGroupRank.OFFICER;
	}
	
	bool CanInvite(string steamId)
	{
		MyGroupMember member = GetMember(steamId);
		if (!member) return false;
		return member.rank >= MyGroupRank.MEMBER;
	}
}

// ============================================================================
// MyGroupManager Class
// ============================================================================


class MyGroupManager
{
	// Singleton
	static ref MyGroupManager s_instance;
	
	// Gruppen-Daten
	ref map<string, ref MyGroup> m_GroupsByTag;
	ref map<string, ref MyGroup> m_PlayerGroups;
	
	// Rate Limiting
	ref map<string, ref array<int>> m_RPCtimestamps;
	
	// Cooldowns
	ref map<string, int> m_InviteCooldowns;
	ref map<string, int> m_KickCooldowns;
	
	// Constructor
	void MyGroupManager()
	{
		Print("[MyGroups] ===================================");
		Print("[MyGroups] GroupManager initializing...");
		Print("[MyGroups] ===================================");
		
		s_instance = this;
		
		m_GroupsByTag = new map<string, ref MyGroup>;
		m_PlayerGroups = new map<string, ref MyGroup>;
		m_RPCtimestamps = new map<string, ref array<int>>;
		m_InviteCooldowns = new map<string, int>;
		m_KickCooldowns = new map<string, int>;
		
		GetDayZGame().Event_OnRPC.Insert(OnRPC);
		
		Print("[MyGroups] GroupManager initialized");
		Print("[MyGroups] ===================================");
	}
	
	// Destructor
	void ~MyGroupManager()
	{
		Print("[MyGroups] GroupManager shutting down...");
		GetDayZGame().Event_OnRPC.Remove(OnRPC);
	}
	
	// Singleton Access
	static MyGroupManager GetInstance()
	{
		return s_instance;
	}
	
	// RPC Handler
	void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
	{
		if (!sender) return;
		
		string steamId = sender.GetId();
		
		if (!CheckRateLimit(steamId))
		{
			SendError(sender, "Too many requests!");
			return;
		}
		
		switch (rpc_type)
		{
			case MyGroupRPCs.GROUP_CREATE:
				HandleGroupCreate(sender, ctx);
				break;
			case MyGroupRPCs.GROUP_DISBAND:
				HandleGroupDisband(sender, ctx);
				break;
			case MyGroupRPCs.GROUP_INVITE:
				HandleGroupInvite(sender, ctx);
				break;
			case MyGroupRPCs.GROUP_ACCEPT_INVITE:
				HandleGroupAcceptInvite(sender, ctx);
				break;
			case MyGroupRPCs.GROUP_LEAVE:
				HandleGroupLeave(sender, ctx);
				break;
			case MyGroupRPCs.GROUP_KICK:
				HandleGroupKick(sender, ctx);
				break;
			case MyGroupRPCs.GROUP_PROMOTE:
				HandleGroupPromote(sender, ctx);
				break;
			case MyGroupRPCs.GROUP_DEMOTE:
				HandleGroupDemote(sender, ctx);
				break;
			case MyGroupRPCs.GROUP_TRANSFER_LEADERSHIP:
				HandleTransferLeadership(sender, ctx);
				break;
		}
	}
	
	// Group Create
	void HandleGroupCreate(PlayerIdentity sender, ParamsReadContext ctx)
	{
		string steamId = sender.GetId();
		
		if (m_PlayerGroups.Contains(steamId))
		{
			SendError(sender, "You are already in a group!");
			return;
		}
		
		Param2<string, string> params;
		if (!ctx.Read(params))
		{
			SendError(sender, "Invalid parameters!");
			return;
		}
		
		string name = params.param1;
		string tag = params.param2;
		
		if (name.Length() < 3 || name.Length() > 30)
		{
			SendError(sender, "Group name must be 3-30 characters!");
			return;
		}
		
		if (tag.Length() < 2 || tag.Length() > 5)
		{
			SendError(sender, "Group tag must be 2-5 characters!");
			return;
		}
		
		if (m_GroupsByTag.Contains(tag))
		{
			SendError(sender, "Group tag already taken!");
			return;
		}
		
		MyGroup group = new MyGroup(tag, name, steamId);
		m_GroupsByTag.Insert(tag, group);
		m_PlayerGroups.Insert(steamId, group);
		
		SendGroupInfo(sender, group);
		SendNotification(sender, "Group '" + name + "' created!", "0xFF00FF00");
		
		Print("[MyGroups] Player " + steamId + " created group: " + tag);
	}
	
	// Group Disband
	void HandleGroupDisband(PlayerIdentity sender, ParamsReadContext ctx)
	{
		string steamId = sender.GetId();
		MyGroup group = GetPlayerGroup(steamId);
		
		if (!group)
		{
			SendError(sender, "You are not in a group!");
			return;
		}
		
		if (group.leaderId != steamId)
		{
			SendError(sender, "Only the leader can disband!");
			return;
		}
		
		foreach (MyGroupMember member : group.members)
		{
			PlayerIdentity memberIdentity = GetIdentityBySteamId(member.steamId);
			if (memberIdentity)
			{
				GetGame().RPCSingleParam(null, MyGroupRPCs.GROUP_MEMBER_LEFT, new Param1<string>("Group disbanded"), true, memberIdentity);
			}
			m_PlayerGroups.Remove(member.steamId);
		}
		
		string groupName = group.name;
		RemoveGroup(group.shortname);
		
		SendNotification(sender, "Group '" + groupName + "' disbanded!", "0xFFFFFF00");
		Print("[MyGroups] Group " + group.shortname + " disbanded by " + steamId);
	}
	
	// Group Invite
	void HandleGroupInvite(PlayerIdentity sender, ParamsReadContext ctx)
	{
		string steamId = sender.GetId();
		
		if (m_InviteCooldowns.Contains(steamId))
		{
			if (GetGame().GetTime() < m_InviteCooldowns.Get(steamId))
			{
				SendError(sender, "Invite cooldown active!");
				return;
			}
		}
		
		MyGroup group = GetPlayerGroup(steamId);
		if (!group)
		{
			SendError(sender, "You are not in a group!");
			return;
		}
		
		if (!group.CanInvite(steamId))
		{
			SendError(sender, "No permission to invite!");
			return;
		}
		
		Param1<string> targetParam;
		if (!ctx.Read(targetParam)) return;
		
		string targetSteamId = targetParam.param1;
		
		if (group.IsMember(targetSteamId))
		{
			SendError(sender, "Player already in group!");
			return;
		}
		
		if (group.IsFull())
		{
			SendError(sender, "Group is full!");
			return;
		}
		
		PlayerIdentity targetIdentity = GetIdentityBySteamId(targetSteamId);
		if (targetIdentity)
		{
			GetGame().RPCSingleParam(null, MyGroupRPCs.GROUP_INVITE_RECEIVED, new Param2<string, string>(group.name, sender.GetName()), true, targetIdentity);
		}
		
		m_InviteCooldowns.Set(steamId, GetGame().GetTime() + 5000);
		SendNotification(sender, "Invite sent!", "0xFF00FF00");
	}
	
	// Group Accept Invite
	void HandleGroupAcceptInvite(PlayerIdentity sender, ParamsReadContext ctx)
	{
		string steamId = sender.GetId();
		
		if (m_PlayerGroups.Contains(steamId))
		{
			SendError(sender, "Already in a group!");
			return;
		}
		
		Param1<string> groupTagParam;
		if (!ctx.Read(groupTagParam)) return;
		
		string groupTag = groupTagParam.param1;
		MyGroup group = GetGroupByTag(groupTag);
		
		if (!group)
		{
			SendError(sender, "Group not found!");
			return;
		}
		
		if (group.IsFull())
		{
			SendError(sender, "Group is full!");
			return;
		}
		
		group.AddMember(steamId, sender.GetName(), MyGroupRank.TRIAL);
		m_PlayerGroups.Insert(steamId, group);
		
		SendGroupInfoToAll(group);
		SendNotification(sender, "Joined: " + group.name, "0xFF00FF00");
		Print("[MyGroups] Player " + steamId + " joined " + groupTag);
	}
	
	// Group Leave
	void HandleGroupLeave(PlayerIdentity sender, ParamsReadContext ctx)
	{
		string steamId = sender.GetId();
		MyGroup group = GetPlayerGroup(steamId);
		
		if (!group)
		{
			SendError(sender, "Not in a group!");
			return;
		}
		
		if (group.leaderId == steamId)
		{
			SendError(sender, "Transfer leadership first!");
			return;
		}
		
		group.RemoveMember(steamId);
		m_PlayerGroups.Remove(steamId);
		
		GetGame().RPCSingleParam(null, MyGroupRPCs.GROUP_MEMBER_LEFT, new Param1<string>(sender.GetName()), true, null);
		SendNotification(sender, "Left group", "0xFFFFFF00");
	}
	
	// Group Kick
	void HandleGroupKick(PlayerIdentity sender, ParamsReadContext ctx)
	{
		string steamId = sender.GetId();
		MyGroup group = GetPlayerGroup(steamId);
		
		if (!group) return;
		if (!group.CanKick(steamId))
		{
			SendError(sender, "No permission!");
			return;
		}
		
		if (m_KickCooldowns.Contains(steamId))
		{
			if (GetGame().GetTime() < m_KickCooldowns.Get(steamId))
			{
				SendError(sender, "Cooldown active!");
				return;
			}
		}
		
		Param1<string> targetParam;
		if (!ctx.Read(targetParam)) return;
		
		string targetSteamId = targetParam.param1;
		MyGroupMember targetMember = group.GetMember(targetSteamId);
		
		if (!targetMember) return;
		if (targetMember.rank >= MyGroupRank.LEADER)
		{
			SendError(sender, "Cannot kick leader!");
			return;
		}
		
		string kickedName = targetMember.playerName;
		group.RemoveMember(targetSteamId);
		m_PlayerGroups.Remove(targetSteamId);
		m_KickCooldowns.Set(steamId, GetGame().GetTime() + 10000);
		
		GetGame().RPCSingleParam(null, MyGroupRPCs.GROUP_MEMBER_KICKED, new Param2<string, string>(kickedName, group.shortname), true, null);
		
		PlayerIdentity kickedIdentity = GetIdentityBySteamId(targetSteamId);
		if (kickedIdentity)
		{
			SendNotification(kickedIdentity, "Kicked from group", "0xFFFF0000");
		}
		
		SendNotification(sender, "Kicked " + kickedName, "0xFF00FF00");
	}
	
	// Group Promote
	void HandleGroupPromote(PlayerIdentity sender, ParamsReadContext ctx)
	{
		string steamId = sender.GetId();
		MyGroup group = GetPlayerGroup(steamId);
		
		if (!group) return;
		
		MyGroupMember senderMember = group.GetMember(steamId);
		if (!senderMember || senderMember.rank < MyGroupRank.LEADER)
		{
			SendError(sender, "Only leader!");
			return;
		}
		
		Param1<string> targetParam;
		if (!ctx.Read(targetParam)) return;
		
		string targetSteamId = targetParam.param1;
		MyGroupMember targetMember = group.GetMember(targetSteamId);
		
		if (!targetMember) return;
		
		MyGroupRank newRank = targetMember.rank + 1;
		if (newRank > MyGroupRank.OFFICER) newRank = MyGroupRank.OFFICER;
		
		targetMember.SetRank(newRank);
		
		GetGame().RPCSingleParam(null, MyGroupRPCs.GROUP_MEMBER_PROMOTED, new Param3<string, string, int>(targetMember.playerName, group.shortname, newRank), true, null);
		SendNotification(sender, "Promoted " + targetMember.playerName, "0xFF00FF00");
	}
	
	// Group Demote
	void HandleGroupDemote(PlayerIdentity sender, ParamsReadContext ctx)
	{
		string steamId = sender.GetId();
		MyGroup group = GetPlayerGroup(steamId);
		
		if (!group) return;
		
		MyGroupMember senderMember = group.GetMember(steamId);
		if (!senderMember || senderMember.rank < MyGroupRank.LEADER)
		{
			SendError(sender, "Only leader!");
			return;
		}
		
		Param1<string> targetParam;
		if (!ctx.Read(targetParam)) return;
		
		string targetSteamId = targetParam.param1;
		MyGroupMember targetMember = group.GetMember(targetSteamId);
		
		if (!targetMember) return;
		if (targetMember.rank <= MyGroupRank.TRIAL)
		{
			SendError(sender, "Cannot demote further!");
			return;
		}
		
		MyGroupRank newRank = targetMember.rank - 1;
		targetMember.SetRank(newRank);
		
		GetGame().RPCSingleParam(null, MyGroupRPCs.GROUP_MEMBER_DEMOTED, new Param3<string, string, int>(targetMember.playerName, group.shortname, newRank), true, null);
		SendNotification(sender, "Demoted " + targetMember.playerName, "0xFF00FF00");
	}
	
	// Transfer Leadership
	void HandleTransferLeadership(PlayerIdentity sender, ParamsReadContext ctx)
	{
		string steamId = sender.GetId();
		MyGroup group = GetPlayerGroup(steamId);
		
		if (!group) return;
		if (group.leaderId != steamId)
		{
			SendError(sender, "Only leader!");
			return;
		}
		
		Param1<string> targetParam;
		if (!ctx.Read(targetParam)) return;
		
		string targetSteamId = targetParam.param1;
		MyGroupMember targetMember = group.GetMember(targetSteamId);
		
		if (!targetMember) return;
		
		string oldLeaderId = group.leaderId;
		group.leaderId = targetSteamId;
		
		MyGroupMember oldLeaderMember = group.GetMember(oldLeaderId);
		if (oldLeaderMember)
		{
			oldLeaderMember.SetRank(MyGroupRank.OFFICER);
		}
		targetMember.SetRank(MyGroupRank.LEADER);
		
		GetGame().RPCSingleParam(null, MyGroupRPCs.GROUP_INFO_UPDATE, new Param3<string, string, string>(group.shortname, oldLeaderId, targetSteamId), true, null);
		SendNotification(sender, "Leadership transferred", "0xFF00FF00");
	}
	
	// Helpers
	MyGroup GetGroupByTag(string tag)
	{
		return m_GroupsByTag.Get(tag);
	}
	
	MyGroup GetPlayerGroup(string steamId)
	{
		return m_PlayerGroups.Get(steamId);
	}
	
	void RemoveGroup(string tag)
	{
		MyGroup group = m_GroupsByTag.Get(tag);
		if (!group) return;
		
		foreach (MyGroupMember member : group.members)
		{
			m_PlayerGroups.Remove(member.steamId);
		}
		
		m_GroupsByTag.Remove(tag);
		Print("[MyGroups] Group " + tag + " removed");
	}
	
	bool CheckRateLimit(string steamId)
	{
		int currentTime = GetGame().GetTime();
		
		if (!m_RPCtimestamps.Contains(steamId))
		{
			m_RPCtimestamps.Set(steamId, new array<int>);
		}
		
		array<int> timestamps = m_RPCtimestamps.Get(steamId);
		
		for (int i = timestamps.Count() - 1; i >= 0; i--)
		{
			if (currentTime - timestamps.Get(i) > 1000)
			{
				timestamps.Remove(i);
			}
		}
		
		if (timestamps.Count() >= 10) return false;
		
		timestamps.Insert(currentTime);
		return true;
	}
	
	PlayerIdentity GetIdentityBySteamId(string steamId)
	{
		return null;
	}
	
	void SendNotification(PlayerIdentity sender, string message, string color)
	{
		if (!sender) return;
		GetGame().RPCSingleParam(null, MyGroupRPCs.GROUP_NOTIFICATION, new Param2<string, string>(message, color), true, sender);
	}
	
	void SendError(PlayerIdentity sender, string message)
	{
		SendNotification(sender, message, "0xFFFF0000");
	}
	
	void SendGroupInfo(PlayerIdentity sender, MyGroup group)
	{
	}
	
	void SendGroupInfoToAll(MyGroup group)
	{
	}
}
