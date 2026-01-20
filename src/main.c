/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glugo-mu <glugo-mu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 18:17:15 by glugo-mu          #+#    #+#             */
/*   Updated: 2026/01/20 11:16:44 by glugo-mu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_cmdlist	*cmd_to_cmdlist(t_cmd *cmd);

static t_token	*process_input(char *input, char ***parts_out, char **orig_out)
{
	char	**parts;
	char	*error;
	t_token	*tokens;

	*orig_out = ft_strdup(input);
	if (!*orig_out || !validate_input(input))
		return (NULL);
	parts = split_input(input);
	*parts_out = parts;
	if (!parts)
		return (NULL);
	error = NULL;
	tokens = tokenize(parts, &error);
	if (!tokens && error)
	{
		printf("minishell: syntax error near unexpected token `%s'\n", error);
		free(error);
	}
	return (tokens);
}

static int	execute_cmd(t_core *core, t_cmd *first, char **my_env)
{
	if (first->next)
		return (execute_pipeline(core, first, my_env));
	if (first->argv && first->argv[0])
	{
		if (isbuiltin(first->argv[0]))
			return (execute_builtin_simple(core, first));
		return (execute_external(core, first, my_env));
	}
	return (0);
}

static void	handle_tokens(t_core *core, t_token *tok, int *es, char **res)
{
	t_cmd	*cmds;

	expand_tokens(tok, core->my_env, *es);
	cmds = commands_from_tokens(tok, NULL);
	core->cmds = cmds;
	token_clear(&tok);
	free_resources(res[0], (char **)res[1], res[2]);
	if (cmds)
	{
		*es = execute_cmd(core, cmds, core->my_env);
		cmd_clear(&cmds);
	}
}

static int	process_loop(t_core *core, int *exit_status)
{
	char	*input;
	char	**parts;
	char	*original;
	t_token	*tokens;
	char	*res[3];

	input = readline("minishell> ");
	if (!input || ft_strcmp(input, "exit") == 0)
		return (free_resources(input, NULL, NULL), 0);
	if (*input)
		add_history(input);
	tokens = process_input(input, &parts, &original);
	if (tokens)
	{
		res[0] = input;
		res[1] = (char *)parts;
		res[2] = original;
		handle_tokens(core, tokens, exit_status, res);
	}
	return (1);
}

int	main(int argc, char **argv, char **envp)
{
	t_core	core;
	char	**my_env;
	int		exit_status;

	core = (t_core){0};
	(void)argc;
	(void)argv;
	exit_status = 0;
	if (!envp || !envp[0])
		my_env = init_env();
	else
		my_env = copy_env(envp);
	if (!my_env)
		return (1);
	init_global_env(&core, my_env);
	core.my_env = my_env;
	setup_signals();
	while (process_loop(&core, &exit_status))
		;
	free_env(core.my_env);
	free_core(&core);
	return (0);
}
