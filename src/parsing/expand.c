/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glugo-mu <glugo-mu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 00:00:00 by glugo-mu          #+#    #+#             */
/*   Updated: 2026/02/02 21:35:11 by glugo-mu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*join_str(char *res, char *val)
{
	char	*tmp;

	tmp = res;
	res = ft_strjoin(res, val);
	free(val);
	return (free(tmp), res);
}

static char	*join_char(char *res, char c)
{
	char	*tmp;

	tmp = res;
	res = ft_strjoin_char(res, c);
	return (free(tmp), res);
}

static char	*expand_variables(const char *str, char **envp, int exit_status,
		int quote_mode)
{
	size_t	i;
	size_t	vl;
	char	*vn;
	char	*result;

	result = ft_strdup("");
	if (!str || !result)
		return (result);
	i = 0;
	while (str[i])
	{
		if (str[i] == '$' && quote_mode != 1)
		{
			vn = extract_var_name(str + i, &vl);
			if (vn && vn[0])
				result = join_str(result, get_var_value(vn, envp, exit_status));
			else if (!vn || !vn[0])
				result = join_char(result, '$');
			free(vn);
			i += vl;
		}
		else
			result = join_char(result, str[i++]);
	}
	return (result);
}

static void	process_word_token(t_token *token, char **envp, int exit_status)
{
	int		quote_mode;
	char	*unquoted;
	char	*expanded;

	unquoted = remove_quotes_and_get_mode(token->value, &quote_mode);
	if (!unquoted)
		return ;
	expanded = expand_variables(unquoted, envp, exit_status, quote_mode);
	free(unquoted);
	if (expanded)
	{
		free(token->value);
		token->value = expanded;
	}
}

void	expand_tokens(t_token *tokens, char **envp, int exit_status)
{
	t_token	*current;

	current = tokens;
	while (current)
	{
		if (current->type == TOKEN_WORD && current->value)
			process_word_token(current, envp, exit_status);
		current = current->next;
	}
}
