---
type: "manual"
---

# Session Automation Configuration

This file defines automated tasks that should be performed at the start of each new agent session.

## Automatic Session Initialization

**IMPLEMENTATION**: The agent automatically reads the `.augment` directory at the start of each session and performs all required automation tasks.

### What Happens Automatically

At the beginning of each new agent session, the agent will:

1. ✅ Automatically read `.augment/session-automation.md` and `.augment/code-generation-rules.md`
2. ✅ Perform comprehensive project analysis
3. ✅ Create session-specific Q&A logging file
4. ✅ Review and apply code generation standards
5. ✅ Provide project context summary

### Benefits of Automatic Initialization

- **Consistent Context**: Every session starts with full project understanding
- **Standards Compliance**: All code generation follows established rules
- **Documentation**: Complete session history for team collaboration
- **Efficiency**: No manual activation required
- **Reliability**: Never forgotten or skipped

## Automatic Tasks

### 1. Project Analysis and Summary
**Trigger**: Start of every new agent session
**Action**: Analyze the project codebase and generate a comprehensive understanding summary

**Analysis Requirements:**
- Review project structure and architecture
- Understand business domain and core concepts
- Identify key modules and their purposes
- Note important conventions and patterns
- Summarize current development context

**Output Format:**
```markdown
# Project Analysis Summary - [DATE]

## Project Overview
[Brief description of the Outcome platform]

## Core Business Domain
[Experiential learning management concepts]

## Key Modules
[List and describe major functional areas]

## Technical Architecture
[Technology stack and patterns]

## Current Context
[Recent changes, active work, important notes]
```

### 2. Session Q&A Logging
**Trigger**: Every question and answer exchange during the session
**Action**: Automatically log all interactions to a session-specific markdown file

**File Naming Convention:**
`questions-and-answers-[YYYY-MM-DD-HHMMSS].md`

**File Location:**
`qanda/` directory in project root

**Content Format:**
```markdown
# Questions and Answers - Session [YYYY-MM-DD HH:MM:SS]

This document contains questions and answers from agent session started on [DATE].

---

### Q1: [Question Title/Summary]
**Question:** [Full user question]

**Answer:** [Complete agent response]

---

### Q2: [Next Question Title/Summary]
**Question:** [Full user question]

**Answer:** [Complete agent response]

---

[Continue for all Q&A pairs in the session]
```

### 3. Code Formatting Compliance
**Trigger**: Any code generation during the session
**Action**: Ensure all generated code follows team formatting standards

**Formatting Standard:**
- **Primary**: OrbisTeamJavaStyle.xml (Eclipse formatter configuration)
- **Scope**: Java, JSP/JSPF, JavaScript, and other code files
- **Requirement**: ALL Augment-generated code must match existing codebase style

**Implementation:**
- Review OrbisTeamJavaStyle.xml formatting rules at session start
- Apply consistent indentation, spacing, and naming conventions
- Match existing code patterns in the project
- Explicitly reference formatting compliance in code generation responses

## Implementation Notes

### Project Analysis
- Should be performed automatically when a new conversation begins
- Use codebase-retrieval tool to gather comprehensive project information
- Focus on understanding business context, not just technical details
- Update analysis if significant project changes are detected

### Q&A Logging
- Log every meaningful question-answer exchange
- Exclude simple acknowledgments or clarifications
- Include code snippets and technical details in answers
- Maintain chronological order within each session
- Create new file for each session to avoid conflicts

### Code Formatting Standards
- **CRITICAL**: All Augment-generated code must follow OrbisTeamJavaStyle.xml formatting
- This Eclipse code formatter configuration is used across the team
- Apply formatting rules to:
  - Java source code
  - JSP/JSPF files
  - JavaScript code
  - Any other code generated during the session
- Ensure consistent indentation, spacing, and style matching existing codebase
- When generating code, explicitly mention adherence to OrbisTeamJavaStyle.xml standards

## File Management
- Keep session files organized in `qanda/` directory
- Consider archiving older session files periodically
- Maintain the main `questions-and-answers.md` as a master reference in project root
- Session files supplement but don't replace the main Q&A file

## Benefits
1. **Consistent Context**: Every session starts with full project understanding
2. **Knowledge Retention**: All interactions are preserved for future reference
3. **Team Collaboration**: Session logs can be shared with team members
4. **Progress Tracking**: Easy to see evolution of questions and solutions over time
5. **Onboarding**: New team members can review session histories for context

---

**AGENT CONFIRMATION**: ✅ Successfully read `.augment/session-automation.md` - Session automation rules loaded and ready for execution.
